/*
 * This file is part of the mod-playerbots module for AzerothCore. See AUTHORS file for Copyright
 * information; released under GNU GPL v2 license, redistribute/modify under version 2 of the License,
 * or (at your option) any later version.
 */

#ifndef PLAYERBOTS_RFKMULTIPLIERS_H
#define PLAYERBOTS_RFKMULTIPLIERS_H

#include "Multiplier.h"

class CharlgaRazorflankMultiplier : public Multiplier
{
public:
    CharlgaRazorflankMultiplier(PlayerbotAI* ai) : Multiplier(ai, "charlga razorflank") {}
    float GetValue(Action* action) override;
};

#endif
