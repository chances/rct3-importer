#ifndef SHAREDRIDE_H_INCLUDED
#define SHAREDRIDE_H_INCLUDED

#include "attraction.h"

namespace r3 {

struct RideStationLimit {
    int32_t x;              ///< Shows: index, skycoaster: 0 or 10
    int32_t z;              ///< skycoaster: 15-18, Shows: 6
    int32_t height;         ///< In height steps
    uint32_t flags;         ///< skycoaster: 49 or 52, 52 confirms to index = 10, Shows: 50
                            /**<
                             * 0x00000001 to -x?
                             * 0x00000002 to +z?
                             * 0x00000004 to +x?
                             * 0x00000008 to -z?
                             * 0x00000010 Entrance
                             * 0x00000020 Exit
                             * 0x00000040 Flip?
                             * 0x00000100 Flip?
                             * 0x00000200 Slide End?
                             * 0x00000400 ... to Lift Hill? 400 alone is between slide end and lift hill. 600 where hill/slide ends
                             **/
};

struct RideOptionOption {
    uint16_t suboption;
    uint16_t group;       ///< Seen 0x0000, 0x0100, 0x0200
};

struct RideOption {
    struct _type_00 {
    };
    struct _type_01 {
        RideOptionOption option;
    };
    struct _type_02 {
        RideOptionOption option;
        float_t unk2;
    };
    /// Guesstimation
    struct _type_03 {
        RideOptionOption option;
        float_t unk2;
    };
    /**
     * Type 4 Option
     * This option sets limits, mainly on rates. The ride option option sets, which
     * rates are affected.
     * For rates it works like this: If the value is below this option, the rates are
     * halved.
     * Probably also determines how moch peeps want to pay and how long they want to
     * wait.
     **/
    struct _type_04 {
        RideOptionOption option;
        float_t unk2;
    };
    /// Guesstimation
    struct _type_05 {
        RideOptionOption option;
        float_t unk2;
    };
    struct _type_06 {
        RideOptionOption option;
        float_t unk2;
    };
    struct _type_07 {
        RideOptionOption option;
        float_t unk2;
    };
    /**
     * Type 8 Option
     * This option sets the ride's rates. The ride option option sets, which
     * value determins application of those rates.
     **/
    struct _type_08 {
        float_t excitement;
        float_t intensity;
        float_t nausea;
        RideOptionOption option;
        float_t factor;       ///< Applied to the above to get the real values
    };
    struct _type_09 {
        float_t unk1;
        float_t unk2;
        float_t unk3;
    };
    struct _type_10 {
        float_t unk1;
        RideOptionOption option;
    };
    struct _type_11 {
        float_t unk1;
        RideOptionOption option;
    };
    struct _type_12 {
        float_t unk1;
        float_t unk2;
        float_t unk3;
    };
    unsigned long type;
    union {
        _type_00 type_00;
        _type_01 type_01;
        _type_02 type_02;
        _type_03 type_03;
        _type_04 type_04;
        _type_05 type_05;
        _type_06 type_06;
        _type_07 type_07;
        _type_08 type_08;
        _type_09 type_09;
        _type_10 type_10;
        _type_11 type_11;
        _type_12 type_12;
    };
};

/// Secondary structure for rides, Vanilla Version
struct Ride_V {
    uint32_t            attractivity_v; ///< For soaked and wild: 0xFFFFFFFF
    uint32_t            seating;
    RideOption**        options;        ///< List terminated by a unrelocated zero pointer
    uint32_t            min_circuits;   ///< Seen 1
    int32_t             max_circuits;   ///< Seen -1, 1 (Spiral Slide, FunHouse, LionShow, TigerShow, DolphinShow, KillerWhaleShow)
                                        /**<
                                         * Probably cycle number. -1 = default? -> 6
                                         */
    uint32_t            entry_fee;
};

/// Secondary structure for rides, Soaked Version
struct Ride_Sext {
    Attraction_S*       att;
    uint32_t            attractivity_sw;///< Seen 55, Reese: 30, Shows: 50, FunHouse: 45. Probably Attractivity
    uint32_t            extra_count;
    RideStationLimit*   extras;         ///< Limits where entrance and exit may be placed
    uint32_t            unk11;          ///< Seen 2, 3
    uint32_t            unk12;          ///< Seen 2, 3
    int32_t             unk13;          ///< Seen -2
    int32_t             unk14;          ///< Seen -2
    int32_t             unk15;          ///< Seen -2
};

struct Ride_S {
    Ride_V              v;
    Ride_Sext           s;
};

struct Ride_Wext {
    uint32_t            unk16;          ///< Seen 0, 1
    uint32_t            unk17;          ///< Seen 0, 1
};

/// Secondary structure for rides, Wild Version
struct Ride_W {
    Ride_V              v;
    Ride_Sext           s;
    Ride_Wext           w;
};

};

#endif // SHAREDRIDE_H_INCLUDED
