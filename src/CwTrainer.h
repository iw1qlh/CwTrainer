enum modes {mode_only_letters, mode_only_numbers, mode_mixed, mode_callsign};

enum states {state_waiting, state_running, state_replay};

enum charTypes {type_letter, type_number, type_unknow};

struct Callsign
{
    const char* prefix;
    int score;
};
