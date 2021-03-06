/********************************************************************
    Copyright (c) 2013-2014 - QSanguosha-Rara

    This file is part of QSanguosha-Hegemony.

    This game is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 3.0
    of the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details.

    See the LICENSE file for more details.

    QSanguosha-Rara
    *********************************************************************/

#include "strategic-advantage.h"
#include "standard-basics.h"
#include "standard-tricks.h"
#include "engine.h"

Blade::Blade(Card::Suit suit, int number)
    : Weapon(suit, number, 3)
{
    setObjectName("Blade");
}

class BladeSkill : public WeaponSkill {
public:
    BladeSkill() : WeaponSkill("Blade") {
        events << TargetChosen << CardFinished;
        frequency = Compulsory;
    }

    virtual QStringList triggerable(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer* &ask_who) const{
        CardUseStruct use = data.value<CardUseStruct>();
        if (triggerEvent == TargetChosen) {
            if (!WeaponSkill::triggerable(use.from))
                return QStringList();

            if (use.to.contains(player) && use.card->isKindOf("Slash") && player->getMark("Equips_of_Others_Nullified_to_You") == 0) {
                ask_who = use.from;
                return QStringList(objectName());
            }
        } else {
            if (use.card->isKindOf("Slash")) {
                foreach (ServerPlayer *p, use.to) {
                    QStringList blade_use = p->property("blade_use").toStringList();
                    if (!blade_use.contains(use.card->toString()))
                        return QStringList();

                    blade_use.removeOne(use.card->toString());
                    room->setPlayerProperty(p, "blade_use", blade_use);

                    if (blade_use.isEmpty())
                        room->removePlayerDisableShow(p, "Blade");
                }
            }
        }
        return QStringList();
    }

    virtual bool effect(TriggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer *) const{
        QStringList blade_use = player->property("blade_use").toStringList();
        CardUseStruct use = data.value<CardUseStruct>();
        if (blade_use.contains(use.card->toString()))
            return false;

        blade_use << use.card->toString();
        room->setPlayerProperty(player, "blade_use", blade_use);

        if (!player->hasShownAllGenerals())
            room->setEmotion(use.from, "weapon/blade");

        room->setPlayerDisableShow(player, "hd", "Blade"); // this effect should always make sense.

        return false;
    }
};

Halberd::Halberd(Card::Suit suit, int number)
    : Weapon(suit, number, 4)
{
    setObjectName("Halberd");
}

Breastplate::Breastplate(Card::Suit suit, int number)
    : Armor(suit, number)
{
    setObjectName("Breastplate");
    transferable = true;
}

class BreastplateSkill : public ArmorSkill {
public:
    BreastplateSkill() : ArmorSkill("Breastplate") {
        events << DamageInflicted;
        frequency = Compulsory;
    }

    virtual QStringList triggerable(TriggerEvent, Room *, ServerPlayer *player, QVariant &data, ServerPlayer* &) const{
        DamageStruct damage = data.value<DamageStruct>();
        if (ArmorSkill::triggerable(player) && damage.damage >= player->getHp()
            && player->getArmor() && player->canDiscard(player, player->getArmor()->getEffectiveId()))
            return QStringList(objectName());
        return QStringList();
    }

    virtual bool cost(TriggerEvent, Room *, ServerPlayer *player, QVariant &, ServerPlayer *) const{
        return player->askForSkillInvoke(objectName());
    }

    virtual bool effect(TriggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer *) const{
        room->throwCard(player->getArmor(), player);
        DamageStruct damage = data.value<DamageStruct>();
        LogMessage log;
        log.type = "#Breastplate";
        log.from = player;
        if (damage.from)
            log.to << damage.from;
        log.arg = QString::number(damage.damage);
        if (damage.nature == DamageStruct::Normal)
            log.arg2 = "normal_nature";
        else if (damage.nature == DamageStruct::Fire)
            log.arg2 = "fire_nature";
        else if (damage.nature == DamageStruct::Thunder)
            log.arg2 = "thunder_nature";
        room->sendLog(log);
        return true;
    }
};

IronArmor::IronArmor(Card::Suit suit, int number)
    : Armor(suit, number)
{
    setObjectName("IronArmor");
}

class IronArmorSkill : public ArmorSkill {
public:
    IronArmorSkill() : ArmorSkill("IronArmor") {
        events << TargetConfirming;
        frequency = Compulsory;
    }

    virtual QStringList triggerable(TriggerEvent, Room *, ServerPlayer *player, QVariant &data, ServerPlayer* &) const{
        if (!ArmorSkill::triggerable(player)) return QStringList();
        CardUseStruct use = data.value<CardUseStruct>();
        if (!use.card) return QStringList();
        if (!use.to.contains(player) || player->getMark("Equips_of_Others_Nullified_to_You") > 0) return QStringList();
        if (use.card->isKindOf("FireAttack") || use.card->isKindOf("FireSlash") || use.card->isKindOf("BurningCamps"))
            return QStringList(objectName());
        return QStringList();
    }

    virtual bool effect(TriggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer *) const{
        CardUseStruct use = data.value<CardUseStruct>();
        LogMessage log2;
        log2.type = "#IronArmor";
        log2.from = player;
        log2.arg = objectName();
        room->sendLog(log2);
        LogMessage log;
        if (use.from) {
            log.type = "$CancelTarget";
            log.from = use.from;
        } else {
            log.type = "$CancelTargetNoUser";
        }
        log.to << player;
        log.arg = use.card->objectName();
        room->sendLog(log);

        room->setEmotion(player, "cancel");

        use.to.removeOne(player);
        data = QVariant::fromValue(use);
        return false;
    }
};

WoodenOxCard::WoodenOxCard() {
    target_fixed = true;
    will_throw = false;
    handling_method = Card::MethodNone;
    m_skillName = "WoodenOx";
}

void WoodenOxCard::use(Room *room, ServerPlayer *source, QList<ServerPlayer *> &) const{
    source->addToPile("wooden_ox", subcards, false);

    QList<ServerPlayer *> targets;
    foreach (ServerPlayer *p, room->getOtherPlayers(source)) {
        if (!p->getTreasure())
            targets << p;
    }
    if (targets.isEmpty())
        return;
    ServerPlayer *target = room->askForPlayerChosen(source, targets, "WoodenOx", "@wooden_ox-move", true);
    if (target) {
        const Card *treasure = source->getTreasure();
        if (treasure)
            room->moveCardTo(treasure, source, target, Player::PlaceEquip,
                             CardMoveReason(CardMoveReason::S_REASON_TRANSFER,
                                            source->objectName(), "WoodenOx", QString()));
    }
}

class WoodenOxSkill: public OneCardViewAsSkill {
public:
    WoodenOxSkill(): OneCardViewAsSkill("WoodenOx") {
        filter_pattern = ".|.|.|hand";
    }

    virtual bool isEnabledAtPlay(const Player *player) const{
        return !player->hasUsed("WoodenOxCard");
    }

    virtual const Card *viewAs(const Card *originalCard) const{
        WoodenOxCard *card = new WoodenOxCard;
        card->addSubcard(originalCard);
        card->setSkillName("WoodenOx");
        return card;
    }
};

class WoodenOxTriggerSkill: public TreasureSkill {
public:
    WoodenOxTriggerSkill(): TreasureSkill("WoodenOx_trigger") {
        events << CardsMoveOneTime;
        global = true;
        frequency = Compulsory;
    }

    virtual bool triggerable(const ServerPlayer *target) const{
        return target != NULL && target->isAlive();
    }

    virtual bool effect(TriggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer *) const{
        CardsMoveOneTimeStruct move = data.value<CardsMoveOneTimeStruct>();
        if (!player->isAlive() || !move.from || move.from != player)
            return false;
        if (player->hasTreasure("WoodenOx")) {
            int count = 0;
            for (int i = 0; i < move.card_ids.size(); i++) {
                if (move.from_pile_names[i] == "wooden_ox") count++;
            }
            if (count > 0) {
                LogMessage log;
                log.type = "#WoodenOx";
                log.from = player;
                log.arg = QString::number(count);
                log.arg2 = "WoodenOx";
                room->sendLog(log);
            }
        }
        if (player->getPile("wooden_ox").isEmpty())
            return false;
        for (int i = 0; i < move.card_ids.size(); i++) {
            if (move.from_places[i] != Player::PlaceEquip && move.from_places[i] != Player::PlaceTable) continue;
            const Card *card = Sanguosha->getEngineCard(move.card_ids[i]);
            if (card->objectName() == "WoodenOx") {
                ServerPlayer *to = qobject_cast<ServerPlayer *>(move.to);
                if (to && to->getTreasure() && to->getTreasure()->objectName() == "WoodenOx"
                    && move.to_place == Player::PlaceEquip) {
                    QList<ServerPlayer *> p_list;
                    p_list << to;
                    to->addToPile("wooden_ox", player->getPile("wooden_ox"), false, p_list);
                } else {
                    player->clearOnePrivatePile("wooden_ox");
                }
                return false;
            }
        }
        return false;
    }
};

WoodenOx::WoodenOx(Suit suit, int number)
    : Treasure(suit, number)
{
    setObjectName("WoodenOx");
}

void WoodenOx::onUninstall(ServerPlayer *player) const{
    player->getRoom()->addPlayerHistory(player, "WoodenOxCard", 0);
    Treasure::onUninstall(player);
}

JadeSeal::JadeSeal(Card::Suit suit, int number)
    : Treasure(suit, number){
    setObjectName("JadeSeal");
}

Drowning::Drowning(Suit suit, int number)
    : SingleTargetTrick(suit, number)
{
    setObjectName("drowning");
}

bool Drowning::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const {
    int total_num = 1 + Sanguosha->correctCardTarget(TargetModSkill::ExtraTarget, Self, this);
    if (targets.length() >= total_num)
        return false;

    return to_select->hasEquip() && to_select != Self;
}

void Drowning::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();
    if (!effect.to->getEquips().isEmpty()
        && room->askForChoice(effect.to, objectName(), "throw+damage", QVariant::fromValue(effect)) == "throw")
        effect.to->throwAllEquips();
    else
        room->damage(DamageStruct(this, effect.from->isAlive() ? effect.from : NULL, effect.to, 1, DamageStruct::Thunder));
}

bool Drowning::isAvailable(const Player *player) const{
    bool canUse = false;
    QList<const Player *> players = player->getAliveSiblings();
    foreach(const Player *p, players) {
        if (player->isProhibited(p, this))
            continue;
        if (!p->hasEquip())
            continue;
        canUse = true;
        break;
    }

    return canUse && TrickCard::isAvailable(player);
}

QStringList Drowning::checkTargetModSkillShow(const CardUseStruct &use) const{
    if (use.to.length() >= 2){
        const ServerPlayer *from = use.from;
        QList<const Skill *> skills = from->getSkillList(false, false);
        QList<const TargetModSkill *> tarmods;

        foreach(const Skill *skill, skills){
            if (from->hasSkill(skill->objectName()) && skill->inherits("TargetModSkill")) {
                const TargetModSkill *tarmod = qobject_cast<const TargetModSkill *>(skill);
                tarmods << tarmod;
            }
        }

        if (tarmods.isEmpty())
            return QStringList();

        int n = use.to.length() - 1;
        QList<const TargetModSkill *> tarmods_copy = tarmods;

        foreach(const TargetModSkill *tarmod, tarmods_copy){
            const Skill *main_skill = Sanguosha->getMainSkill(tarmod->objectName());
            if (from->hasShownSkill(main_skill)){
                tarmods.removeOne(tarmod);
                n -= tarmod->getExtraTargetNum(from, this);
            }
        }

        if (tarmods.isEmpty() || n <= 0)
            return QStringList();

        tarmods_copy = tarmods;

        QStringList shows;
        foreach(const TargetModSkill *tarmod, tarmods_copy){
            const Skill *main_skill = Sanguosha->getMainSkill(tarmod->objectName());
            shows << main_skill->objectName();
        }
        return shows;
    }
    return QStringList();
}

BurningCamps::BurningCamps(Card::Suit suit, int number, bool is_transferable)
    : AOE(suit, number)
{
    setObjectName("burning_camps");
    transferable = is_transferable;
}

bool BurningCamps::isAvailable(const Player *player) const{
    bool canUse = false;
    QList<const Player *> players = player->getNextAlive()->getFormation();
    foreach (const Player *p, players) {
        if (player->isProhibited(p, this))
            continue;

        canUse = true;
        break;
    }

    return canUse && TrickCard::isAvailable(player);
}

void BurningCamps::onUse(Room *room, const CardUseStruct &card_use) const{
    CardUseStruct new_use = card_use;
    QList<const Player *> targets = card_use.from->getNextAlive()->getFormation();
    foreach (const Player *player, targets) {
        const Skill *skill = room->isProhibited(card_use.from, player, this);
        ServerPlayer *splayer = room->findPlayer(player->objectName());
        if (skill) {
            if (!skill->isVisible())
                skill = Sanguosha->getMainSkill(skill->objectName());
            if (skill->isVisible()) {
                LogMessage log;
                log.type = "#SkillAvoid";
                log.from = splayer;
                log.arg = skill->objectName();
                log.arg2 = objectName();
                room->sendLog(log);

                room->broadcastSkillInvoke(skill->objectName());
            }
        } else
            new_use.to << splayer;
    }

    TrickCard::onUse(room, new_use);
}

void BurningCamps::onEffect(const CardEffectStruct &effect) const {
    effect.to->getRoom()->damage(DamageStruct(this, effect.from, effect.to, 1, DamageStruct::Fire));
}

LureTiger::LureTiger(Card::Suit suit, int number, bool is_transferable)
    : TrickCard(suit, number)
{
    setObjectName("lure_tiger");
    transferable = is_transferable;
}

QString LureTiger::getSubtype() const{
    return "lure_tiger";
}

bool LureTiger::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    int total_num = 2 + Sanguosha->correctCardTarget(TargetModSkill::ExtraTarget, Self, this);
    if (targets.length() >= total_num)
        return false;
    if (Self->isCardLimited(this, Card::MethodUse))
        return false;

    return to_select != Self;
}

void LureTiger::use(Room *room, ServerPlayer *source, QList<ServerPlayer *> &targets) const{
    foreach(ServerPlayer *target, targets) {
        CardEffectStruct effect;
        effect.card = this;
        effect.from = source;
        effect.to = target;

        room->cardEffect(effect);
    }

    source->drawCards(1, objectName());

    QList<int> table_cardids = room->getCardIdsOnTable(this);
    if (!table_cardids.isEmpty()) {
        DummyCard dummy(table_cardids);
        CardMoveReason reason(CardMoveReason::S_REASON_USE, source->objectName(), QString(), this->getSkillName(), QString());
        if (targets.size() == 1) reason.m_targetId = targets.first()->objectName();
        room->moveCardTo(&dummy, source, NULL, Player::DiscardPile, reason, true);
    }
}

void LureTiger::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();

    room->setPlayerCardLimitation(effect.to, "use", ".", false);
    room->setPlayerProperty(effect.to, "removed", true);
    effect.from->setFlags("LureTigerUser");
}

QStringList LureTiger::checkTargetModSkillShow(const CardUseStruct &use) const{
    if (use.to.length() >= 3){
        const ServerPlayer *from = use.from;
        QList<const Skill *> skills = from->getSkillList(false, false);
        QList<const TargetModSkill *> tarmods;

        foreach(const Skill *skill, skills){
            if (from->hasSkill(skill->objectName()) && skill->inherits("TargetModSkill")){
                const TargetModSkill *tarmod = qobject_cast<const TargetModSkill *>(skill);
                tarmods << tarmod;
            }
        }

        if (tarmods.isEmpty())
            return QStringList();

        int n = use.to.length() - 2;
        QList<const TargetModSkill *> tarmods_copy = tarmods;

        foreach(const TargetModSkill *tarmod, tarmods_copy){
            const Skill *main_skill = Sanguosha->getMainSkill(tarmod->objectName());
            if (from->hasShownSkill(main_skill)){
                tarmods.removeOne(tarmod);
                n -= tarmod->getExtraTargetNum(from, this);
            }
        }

        if (tarmods.isEmpty() || n <= 0)
            return QStringList();

        tarmods_copy = tarmods;

        QStringList shows;
        foreach(const TargetModSkill *tarmod, tarmods_copy){
            const Skill *main_skill = Sanguosha->getMainSkill(tarmod->objectName());
            shows << main_skill->objectName();
        }
        return shows;
    }
    return QStringList();
}

class LureTigerSkill : public TriggerSkill {
public:
    LureTigerSkill() : TriggerSkill("lure_tiger_effect") {
        events << Death << EventPhaseChanging;
        global = true;
    }

    virtual QStringList triggerable(TriggerEvent triggerEvent, Room *room, ServerPlayer *player, QVariant &data, ServerPlayer* &) const{
        if (!player->hasFlag("LureTigerUser"))
            return QStringList();
        if (triggerEvent == EventPhaseChanging) {
            PhaseChangeStruct change = data.value<PhaseChangeStruct>();
            if (change.to != Player::NotActive)
                return QStringList();
        } else if (triggerEvent == Death) {
            DeathStruct death = data.value<DeathStruct>();
            if (death.who != player)
                return QStringList();
        }

        foreach (ServerPlayer *p, room->getOtherPlayers(player))
            if (p->isRemoved()) {
                room->setPlayerProperty(p, "removed", false);
                room->removePlayerCardLimitation(p, "use", ".$0");
            }

        return QStringList();
    }
};

class LureTigerProhibit : public ProhibitSkill {
public:
    LureTigerProhibit() : ProhibitSkill("#lure_tiger-prohibit") {
    }

    virtual bool isProhibited(const Player *, const Player *to, const Card *card, const QList<const Player *> &) const{
        return to->isRemoved() && card->getTypeId() != Card::TypeSkill;
    }
};

FightTogether::FightTogether(Card::Suit suit, int number)
    : GlobalEffect(suit, number)
{
    setObjectName("fight_together");
}

bool FightTogether::isAvailable(const Player *player) const{
    if (player->hasFlag("Global_FightTogetherFailed"))
        return false;
    QHash<QString, QStringList> kingdoms = player->getBigAndSmallKingdoms(objectName());
    bool invoke = !kingdoms["big"].isEmpty() && !kingdoms["small"].isEmpty();
    return (invoke || (player->hasLordSkill("hongfa") && !player->getPile("heavenly_army").isEmpty())) // HongfaTianbing
        && GlobalEffect::isAvailable(player);
}

void FightTogether::onUse(Room *room, const CardUseStruct &card_use) const{
    ServerPlayer *source = card_use.from;
    QHash<QString, QStringList> kingdoms = source->getBigAndSmallKingdoms(objectName(), MaxCardsType::Normal);
    if (kingdoms["big"].isEmpty() || kingdoms["small"].isEmpty()) {
        room->setPlayerFlag(source, "Global_FightTogetherFailed");
        return;
    }
    QList<ServerPlayer *> bigs, smalls;
    foreach (ServerPlayer *p, room->getAllPlayers()) {
        const Skill *skill = room->isProhibited(source, p, this);
        if (skill) {
            if (!skill->isVisible())
                skill = Sanguosha->getMainSkill(skill->objectName());
            if (skill->isVisible()) {
                LogMessage log;
                log.type = "#SkillAvoid";
                log.from = p;
                log.arg = skill->objectName();
                log.arg2 = objectName();
                room->sendLog(log);

                room->broadcastSkillInvoke(skill->objectName());
            }
            continue;
        }
        QString kingdom = p->objectName();
        if (kingdoms["big"].length() == 1 && kingdoms["big"].first().startsWith("sgs")) { // for JadeSeal
            if (kingdoms["big"].contains(kingdom))
                bigs << p;
            else
                smalls << p;
        } else {
            if (!p->hasShownOneGeneral())
                kingdom = "anjiang";
            else if (p->getRole() == "careerist")
                kingdom = "careerist";
            else
                kingdom = p->getKingdom();
            if (kingdoms["big"].contains(kingdom))
                bigs << p;
            else if (kingdoms["small"].contains(kingdom))
                smalls << p;
        }
    }
    QStringList choices;
    if (!bigs.isEmpty())
        choices << "big";
    if (!smalls.isEmpty())
        choices << "small";

    Q_ASSERT(!choices.isEmpty());

    QString choice = room->askForChoice(source, objectName(), choices.join("+"));

    CardUseStruct use = card_use;
    if (choice == "big")
        use.to = bigs;
    else if (choice == "small")
        use.to = smalls;
    Q_ASSERT(!use.to.isEmpty());
    TrickCard::onUse(room, use);
}

void FightTogether::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.from->getRoom();
    if (!effect.to->isChained()) {
        effect.to->setChained(true);
        room->setEmotion(effect.to, "chain");
        room->broadcastProperty(effect.to, "chained");
        room->getThread()->trigger(ChainStateChanged, room, effect.to);
    } else
        effect.to->drawCards(1);
}

AllianceFeast::AllianceFeast(Card::Suit suit, int number)
    : AOE(suit, number)
{
    setObjectName("alliance_feast");
    target_fixed = false;
}

bool AllianceFeast::targetFilter(const QList<const Player *> &targets, const Player *to_select, const Player *Self) const{
    if (!targets.isEmpty())
        return false;
    return to_select->hasShownOneGeneral() && !Self->isFriendWith(to_select);
}

void AllianceFeast::onUse(Room *room, const CardUseStruct &card_use) const{
    ServerPlayer *source = card_use.from;
    QList<ServerPlayer *> targets;
    if (!source->isProhibited(source, this))
        targets << source;
    if (card_use.to.length() == 1) {
        ServerPlayer *target = card_use.to.first();
        QList<ServerPlayer *> other_players = room->getOtherPlayers(source);
        foreach (ServerPlayer *player, other_players) {
            if (!target->isFriendWith(player))
                continue;
            const Skill *skill = room->isProhibited(source, player, this);
            if (skill) {
                if (!skill->isVisible())
                    skill = Sanguosha->getMainSkill(skill->objectName());
                if (skill->isVisible()) {
                    LogMessage log;
                    log.type = "#SkillAvoid";
                    log.from = player;
                    log.arg = skill->objectName();
                    log.arg2 = objectName();
                    room->sendLog(log);

                    room->broadcastSkillInvoke(skill->objectName());
                }
            } else
                targets << player;
        }
    } else
        targets = card_use.to;

    CardUseStruct use = card_use;
    use.to = targets;
    TrickCard::onUse(room, use);
}

void AllianceFeast::use(Room *room, ServerPlayer *source, QList<ServerPlayer *> &targets) const{
    foreach(ServerPlayer *target, targets) {
        CardEffectStruct effect;
        effect.card = this;
        effect.from = source;
        effect.to = target;

        if (target == source) {
            int n = 0;
            ServerPlayer *enemy = targets.last();
            foreach (ServerPlayer *p, room->getOtherPlayers(source)) {
                if (enemy->isFriendWith(p))
                    ++n;
            }
            target->setMark(objectName(), n);
        }
        room->cardEffect(effect);
        target->setMark(objectName(), 0);
    }

    QList<int> table_cardids = room->getCardIdsOnTable(this);
    if (!table_cardids.isEmpty()) {
        DummyCard dummy(table_cardids);
        CardMoveReason reason(CardMoveReason::S_REASON_USE, source->objectName(), QString(), this->getSkillName(), QString());
        room->moveCardTo(&dummy, source, NULL, Player::DiscardPile, reason, true);
    }
}

void AllianceFeast::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();
    if (effect.to->getMark(objectName()) > 0) {
        effect.to->drawCards(effect.to->getMark(objectName()), objectName());
    } else {
        QStringList choices;
        if (effect.to->isWounded())
            choices << "recover";
        choices << "draw";
        QString choice = room->askForChoice(effect.to, objectName(), choices.join("+"));
        if (choice == "recover") {
            RecoverStruct recover;
            recover.who = effect.from;
            room->recover(effect.to, recover);
        } else {
            effect.to->drawCards(1, objectName());
            if (effect.to->isChained()) {
                effect.to->setChained(false);
                room->setEmotion(effect.to, "chain");
                room->broadcastProperty(effect.to, "chained");
                room->getThread()->trigger(ChainStateChanged, room, effect.to);
            }
        }
    }
}

bool AllianceFeast::isAvailable(const Player *player) const{
    return player->hasShownOneGeneral() && !player->isProhibited(player, this);
}

ThreatenEmperor::ThreatenEmperor(Suit suit, int number)
    : SingleTargetTrick(suit, number)
{
    setObjectName("threaten_emperor");
    target_fixed = true;
    transferable = true;
}

void ThreatenEmperor::onUse(Room *room, const CardUseStruct &card_use) const{
    CardUseStruct use = card_use;
    if (use.to.isEmpty())
        use.to << use.from;
    SingleTargetTrick::onUse(room, use);
}

bool ThreatenEmperor::isAvailable(const Player *player) const{
    if (!player->hasShownOneGeneral())
        return false;
    QHash<QString, QStringList> kingdoms = player->getBigAndSmallKingdoms(objectName(), MaxCardsType::Max);
    bool invoke = !kingdoms["big"].isEmpty() && !kingdoms["small"].isEmpty();
    if (invoke) {
        if (kingdoms["big"].length() == 1 && kingdoms["big"].first().startsWith("sgs")) // for JadeSeal
            invoke = kingdoms["big"].contains(player->objectName());
        else {
            QString kingdom = player->getRole() == "careerist" ? "careerist" : player->getKingdom();
            invoke = kingdoms["big"].contains(kingdom);
        }
    }
    return invoke && !player->isProhibited(player, this) && TrickCard::isAvailable(player);
}

void ThreatenEmperor::onEffect(const CardEffectStruct &effect) const{
    if (effect.from->getPhase() == Player::Play)
        effect.from->setFlags("Global_PlayPhaseTerminated");
    effect.to->setMark("ThreatenEmperorExtraTurn", 1);
}

class ThreatenEmperorSkill : public TriggerSkill {
public:
    ThreatenEmperorSkill() : TriggerSkill("threaten_emperor") {
        events << EventPhaseStart;
        global = true;
    }

    virtual int getPriority() const{
        return 1;
    }

    virtual QMap<ServerPlayer *, QStringList> triggerable(TriggerEvent, Room *room, ServerPlayer *player, QVariant &) const{
        QMap<ServerPlayer *, QStringList> list;
        if (player->getPhase() != Player::NotActive)
            return list;
        foreach (ServerPlayer *p, room->getAllPlayers())
            if (p->getMark("ThreatenEmperorExtraTurn") > 0)
                list.insert(p, QStringList(objectName()));
   
        return list;
    }

    virtual bool cost(TriggerEvent, Room *room, ServerPlayer *, QVariant &data, ServerPlayer *ask_who) const{
        ask_who->removeMark("ThreatenEmperorExtraTurn");
        return room->askForCard(ask_who, ".", "@threaten_emperor", data, objectName());
    }

    virtual bool effect(TriggerEvent, Room *, ServerPlayer *, QVariant &, ServerPlayer *ask_who) const{
        ask_who->gainAnExtraTurn();
        return false;
    }
};

ImperialOrder::ImperialOrder(Suit suit, int number)
    : GlobalEffect(suit, number)
{
    setObjectName("imperial_order");
}

bool ImperialOrder::isAvailable(const Player *player) const{
    bool invoke = !player->hasShownOneGeneral();
    if (!invoke) {
        foreach (const Player *p, player->getAliveSiblings()) {
            if (!p->hasShownOneGeneral() && !player->isProhibited(p, this)) {
                invoke = true;
                break;
            }
        }
    }
    return invoke && TrickCard::isAvailable(player);
}

void ImperialOrder::onUse(Room *room, const CardUseStruct &card_use) const{
    ServerPlayer *source = card_use.from;
    QList<ServerPlayer *> targets;
    foreach (ServerPlayer *p, room->getAllPlayers()) {
        if (p->hasShownOneGeneral())
            continue;
        const Skill *skill = room->isProhibited(source, p, this);
        if (skill) {
            if (!skill->isVisible())
                skill = Sanguosha->getMainSkill(skill->objectName());
            if (skill && skill->isVisible()) {
                LogMessage log;
                log.type = "#SkillAvoid";
                log.from = p;
                log.arg = skill->objectName();
                log.arg2 = objectName();
                room->sendLog(log);

                room->broadcastSkillInvoke(skill->objectName());
            }
            continue;
        }
        targets << p;
    }

    CardUseStruct use = card_use;
    use.to = targets;
    Q_ASSERT(!use.to.isEmpty());
    TrickCard::onUse(room, use);
}

void ImperialOrder::use(Room *room, ServerPlayer *source, QList<ServerPlayer *> &targets) const{
    room->setCardFlag(this, "imperial_order_normal_use");
    GlobalEffect::use(room, source, targets);
}

void ImperialOrder::onEffect(const CardEffectStruct &effect) const{
    Room *room = effect.to->getRoom();
    if (room->askForCard(effect.to, "EquipCard", "@imperial_order-equip"))
        return;
    QStringList choices;
    if (!effect.to->hasShownAllGenerals()
        && ((!effect.to->hasShownGeneral1() && effect.to->disableShow(true).isEmpty())
            || (effect.to->getGeneral2() && !effect.to->hasShownGeneral2() && effect.to->disableShow(false).isEmpty())))
        choices << "show";
    choices << "losehp";
    QString choice = room->askForChoice(effect.to, objectName(), choices.join("+"));
    if (choice == "show") {
        effect.to->askForGeneralShow();
        effect.to->drawCards(1, objectName());
    } else {
        room->loseHp(effect.to);
    }
}

StrategicAdvantagePackage::StrategicAdvantagePackage()
    : Package("strategic_advantage", Package::CardPack){
    QList<Card *> cards;

    cards
        // basics
        // -- spade
        << new Slash(Card::Spade, 4)
        << new Analeptic(Card::Spade, 6, true) // transfer
        << new Slash(Card::Spade, 7)
        << new Slash(Card::Spade, 8)
        << new ThunderSlash(Card::Spade, 9)
        << new ThunderSlash(Card::Spade, 10)
        << new ThunderSlash(Card::Spade, 11, true) // transfer
        // -- heart
        << new Jink(Card::Heart, 4)
        << new Jink(Card::Heart, 5)
        << new Jink(Card::Heart, 6)
        << new Jink(Card::Heart, 7)
        << new Peach(Card::Heart, 8)
        << new Peach(Card::Heart, 9)
        << new Slash(Card::Heart, 10)
        << new Slash(Card::Heart, 11)
        // -- club
        << new Slash(Card::Club, 4)
        << new ThunderSlash(Card::Club, 5, true) // transfer
        << new Slash(Card::Club, 6)
        << new Slash(Card::Club, 7)
        << new Slash(Card::Club, 8)
        << new Analeptic(Card::Club, 9)
        // -- diamond
        << new Peach(Card::Diamond, 2)
        << new Peach(Card::Diamond, 3, true) // transfer
        << new Jink(Card::Diamond, 6)
        << new Jink(Card::Diamond, 7)
        << new FireSlash(Card::Diamond, 8)
        << new FireSlash(Card::Diamond, 9)
        << new Jink(Card::Diamond, 13)

        // tricks
        // -- spade
        << new ThreatenEmperor(Card::Spade, 1) // transfer
        << new BurningCamps(Card::Spade, 3, true) // transfer
        << new FightTogether(Card::Spade, 12)
        << new Nullification(Card::Spade, 13)
        // -- heart
        << new AllianceFeast()
        << new LureTiger(Card::Heart, 2)
        << new BurningCamps(Card::Heart, 12)
        << new Drowning(Card::Heart, 13)
        // -- club
        << new ImperialOrder(Card::Club, 3)
        << new FightTogether(Card::Club, 10)
        << new BurningCamps(Card::Club, 11)
        << new Drowning(Card::Club, 12)
        << new HegNullification(Card::Club, 13)
        // -- diamond
        << new ThreatenEmperor(Card::Diamond, 1) // transfer
        << new ThreatenEmperor(Card::Diamond, 4) // transfer
        << new LureTiger(Card::Diamond, 10, true) // transfer
        << new HegNullification(Card::Diamond, 11)

        // equips
        << new IronArmor()
        << new Blade(Card::Spade, 5);
    Horse *horse = new OffensiveHorse(Card::Heart, 3, -1, true); // transfer
    horse->setObjectName("JingFan");
    cards
        << horse
        << new JadeSeal(Card::Club, 1)
        << new Breastplate() // transfer
        << new WoodenOx(Card::Diamond, 5)
        << new Halberd(Card::Diamond, 12);

    skills << new BladeSkill
           << new BreastplateSkill
           << new IronArmorSkill
           << new WoodenOxSkill << new WoodenOxTriggerSkill
           << new LureTigerSkill << new LureTigerProhibit
           << new ThreatenEmperorSkill;
    insertRelatedSkills("lure_tiger_effect", "#lure_tiger-prohibit");

    foreach (Card *card, cards)
        card->setParent(this);

    addMetaObject<WoodenOxCard>();
}

ADD_PACKAGE(StrategicAdvantage)
