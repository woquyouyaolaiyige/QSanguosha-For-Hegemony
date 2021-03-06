-- translation for StrategicAdvantage Package

return {
	["strategic_advantage"] = "势备篇",

	["transfer"] = "连横",

	["Blade"] = "青龙偃月刀",
	[":Blade"] = "装备牌·武器\n\n攻击范围：3\n技能：锁定技，每当你使用【杀】指定一个目标后，其不能明置武将直到此【杀】结算完毕。",

	["Halberd"] = "方天画戟",
	[":Halberd"] = "装备牌·武器\n\n攻击范围：4\n技能：君主技，限定技，锁定技，主将技，副将技，阵法技，出牌阶段限一次，Fs了不起。",
	["#HalberdUse"] = "%from 发动了【<font color=\"yellow\"><b>方天画戟</b></font>】效果",

	["Breastplate"] = "护心镜",
	[":Breastplate"] = "装备牌·防具\n\n技能：每当你受到不小于你的体力值的伤害时，你可以弃置装备区中的此牌，防止此伤害。",
	["#Breastplate"] = "%from 防止了 %to 对其造成的 %arg 点伤害[%arg2]",

	["IronArmor"] = "明光铠",
	[":IronArmor"] = "装备牌·防具\n\n技能：每当你成为【火攻】、火【杀】或【火烧连营】的目标时，你取消自己。",
	["#IronArmor"] = "%from 的装备技能【%arg】被触发",

	["WoodenOx"] = "木牛流马",
	[":WoodenOx"] = "装备牌·宝物\n\n技能：\n" ..
					"1. 出牌阶段限一次，你可以将一张手牌置于【木牛流马】下，若如此做，你可以将【木牛流马】移动至一名其他角色的装备区。\n" ..
					"2. 你可以将【木牛流马】下的牌视为手牌使用或打出。\n" ..
					"◆每当你失去装备区的【木牛流马】后，若【木牛流马】未移动至装备区，其下的牌置入弃牌堆，否则这些牌仍置于【木牛流马】下。\n◆【木牛流马】下的牌为移出游戏。",
	["@wooden_ox-move"] = "你可以将【木牛流马】移动至一名其他角色的装备区",
	["wooden_ox"] = "木牛流马",
	["#WoodenOx"] = "%from 使用/打出了 %arg 张 %arg2 牌",

	["JadeSeal"] = "玉玺",
	[":JadeSeal"] = "装备牌·宝物\n\n技能：君主技，限定技，锁定技，主将技，副将技，阵法技，出牌阶段限一次，你可以砍了啦啦。",

	["drowning"] = "水淹七军",
	[":drowning"] = "锦囊牌\n\n使用时机：出牌阶段。\n使用目标：一名装备区里有牌的其他角色。\n作用效果：目标角色选择一项：1.弃置装备区里的所有牌；2.受到你造成的1点雷电伤害。",
	["drowning:throw"] = "弃置装备区里的所有牌",
	["drowning:damage"] = "受到其造成的1点雷电伤害",

	["burning_camps"] = "火烧连营",
	[":burning_camps"] = "锦囊牌\n\n使用时机：出牌阶段。\n使用目标：你的下家和除其外与其处于同一队列的所有角色。\n作用效果：目标角色受到你造成的1点火焰伤害。",

	["lure_tiger"] = "调虎离山",
	[":lure_tiger"] = "锦囊牌\n\n使用时机：出牌阶段。\n使用目标：至多两名其他角色。\n作用效果：目标角色于此回合结束之前不计入距离和座次计算且不能使用牌且不是牌的合法目标。\n执行动作：此牌结算结束时，你摸一张牌。",
	["lure_tiger_effect"] = "调虎离山",
	["#lure_tiger-prohibit"] = "调虎离山",

	["fight_together"] = "勠力同心",
	[":fight_together"] = "锦囊牌\n\n使用时机：出牌阶段。\n使用目标：所有大势力角色或所有小势力角色。\n作用效果：若目标角色：不处于连环状态，其横置；处于连换状态，其摸一张牌。",
	["fight_together:big"] = "大势力",
	["fight_together:small"] = "小势力",

	["alliance_feast"] = "联军盛宴",
	[":alliance_feast"] = "锦囊牌\n\n使用时机：出牌阶段。\n使用目标：你和你选择的除你的势力外的一个势力的所有角色。\n作用效果：若目标角色：为你，你摸X张牌（X为该势力的角色数）；不为你，其选择一项：1. 回复1点体力；2. 摸一张牌，然后重置。",

	["threaten_emperor"] = "挟天子以令诸侯",
	[":threaten_emperor"] = "锦囊牌\n\n使用时机：出牌阶段。\n使用目标：为大势力角色的你。\n作用效果：你结束出牌阶段，若如此做，此回合结束时，你可以弃置一张牌，获得一个额外的回合。",
	["@threaten_emperor"] = "受到【挟天子以令诸侯】影响，你可以弃置一张牌，获得一个额外的回合",

	["imperial_order"] = "敕令",
	[":imperial_order"] = "锦囊牌\n\n使用时机：出牌阶段。\n使用目标：所有没有势力的角色。\n作用效果：目标角色选择一项：1. 明置一张武将牌，然后摸一张牌；2. 弃置一张装备牌；3. 失去1点体力。\n\n※若此牌未因使用此效果而进入弃牌堆时，则改为将此牌移出游戏，然后于此回合结束时视为对所有未确定势力的角色使用此牌。",
	["@imperial_order-equip"] = "受到【敕令】的影响，你需要弃置一张装备牌，或点“取消”选择以下一项：<br />1. 明置一张武将牌，然后摸一张牌<br />2. 失去1点体力",
	["imperial_order:show"] = "明置一张武将牌，然后摸一张牌",
	["imperial_order:losehp"] = "失去1点体力",
	["#RemoveImperialOrder"] = "【%arg】因使用其效果以外的原因进入弃牌堆，将被移出游戏",
	["#ImperialOrderEffect"] = "%from 的回合结束，【%arg】因被移出游戏生效",
}