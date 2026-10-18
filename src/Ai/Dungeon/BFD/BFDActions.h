/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_BFDACTIONS_H
#define PLAYERBOTS_BFDACTIONS_H

#include "Action.h"
#include "BFDTriggers.h"
#include "MovementActions.h"
#include "PlayerbotAI.h"
#include "Playerbots.h"

class BfdMoveAroundBossAction : public MovementAction
{
public:
    BfdMoveAroundBossAction(PlayerbotAI* ai, std::string const name) : MovementAction(ai, name) {}

protected:
    // To `angle` from the boss's facing, `distance` yards from it.
    bool MoveAround(Unit* boss, float angle, float distance);
};

class BfdMoveFromTorrentAction : public BfdMoveAroundBossAction
{
public:
    BfdMoveFromTorrentAction(PlayerbotAI* ai) : BfdMoveAroundBossAction(ai, "bfd move from torrent") {}
    bool Execute(Event event) override;
};

class BfdMoveFromFrontalConeAction : public BfdMoveAroundBossAction
{
public:
    BfdMoveFromFrontalConeAction(PlayerbotAI* ai) : BfdMoveAroundBossAction(ai, "bfd move from frontal cone") {}
    bool Execute(Event event) override;
};

#endif
