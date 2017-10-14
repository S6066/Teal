teal_character =
{
    codename = "villager",
    size = { 113, 99 },
    sprite = ":/game/char/villager", -- todo: load animations (lua?)
    defgfxpos = { -25, -66 },
    deflgcpos = { 1, 1 },
    maxhealth = 100,
    random = { false },
    name = "Villager",

    attack =
    {
        { "neutral", 10 }
    },

    resistance =
    {
        { "neutral", 10 }
    },

    fight =
    {
        autoAttack = false,
        movementPoints = 3,
        actionPoints = 6
    }
}
