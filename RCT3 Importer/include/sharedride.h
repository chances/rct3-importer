#ifndef SHAREDRIDE_H_INCLUDED
#define SHAREDRIDE_H_INCLUDED

namespace r3 {

struct RideExtra {
    unsigned long index;        ///< Shows: index, skycoaster: 0 or 10
    unsigned long unk2;         ///< skycoaster: 15-18, Shows: 6
    unsigned long unk3;         ///< skycoaster, Shows: 0
    unsigned long unk4;         ///< skycoaster: 49 or 52, 52 confirms to index = 10, Shows: 50
};

struct RideOptionOption {
    unsigned short suboption;
    unsigned short group;       ///< Seen 0x0000, 0x0100, 0x0200
};

struct RideOption {
    unsigned long type;
    union data {
        struct type_00 {
        };
        struct type_02 {
            RideOptionOption option;
            float unk2;
        };
        struct type_04 {
            RideOptionOption option;
            float unk2;
        };
        struct type_07 {
            RideOptionOption option;
            float unk2;
        };
        struct type_08 {
            float excitement;
            float intensity;
            float nausea;
            RideOptionOption option;
            float factor;       ///< Applied to the above to get the real values
        };
        struct type_09 {
            float unk1;
            float unk2;
            float unk3;
        };
        struct type_10 {
            float unk1;
            RideOptionOption option;
        };
        struct type_11 {
            float unk1;
            RideOptionOption option;
        };
        struct type_12 {
            float unk1;
            float unk2;
            float unk3;
        };
    };
};

/// Secondary structure for rides, Soaked Version
struct Ride_S {
    uint32_t            unk1;           ///< always 0xFFFFFFFF
    uint32_t            seating;
    RideOption**        options;        ///< List terminated by a unrelocated zero pointer
    uint32_t            unk4;           ///< Seen 1
    int32_t             unk5;           ///< Seen -1, 1 (FunHouse, LionShow, TigerShow, DolphinShow, KillerWhaleShow)
    uint32_t            entry_fee;
    Attraction_S*       att;
    uint32_t            unk8;           ///< Seen 55, Reese: 30, Shows: 50, FunHouse: 45. Probably Attractivity
    uint32_t            extra_count;
    RideExtra*          extras;         ///< Extras have: Soaked Shows, skycoaster
    uint32_t            unk11;          ///< Seen 3
    uint32_t            unk12;          ///< Seen 3
    int32_t             unk13;          ///< Seen -2
    int32_t             unk14;          ///< Seen -2
    int32_t             unk15;          ///< Seen -2
};

struct Ride_Wext {
    uint32_t            unk16;          ///< Seen 1
    uint32_t            unk17;          ///< Seen 1
};

/// Secondary structure for rides, Wild Version
struct Ride_W {
    Ride_S              s;
    Ride_Wext           w;
};

};

#endif // SHAREDRIDE_H_INCLUDED
