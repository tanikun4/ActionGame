#pragma once

struct ActionState {
    bool current = false;
    bool previous = false;

    inline void Update(bool down) {
        previous = current;
        current = down;
    }

    inline bool Press()   const { return current; }
    inline bool Trigger() const { return current && !previous; }
    inline bool Release() const { return !current && previous; }
};

