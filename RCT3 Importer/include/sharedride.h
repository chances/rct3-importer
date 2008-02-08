#ifndef SHAREDRIDE_H_INCLUDED
#define SHAREDRIDE_H_INCLUDED

struct SharedRide {
    /// Seating enum
    struct RCT3_Seating {
        enum {
            Stand =                    0x00,
            Sit =                      0x01,
            Sledge =                   0x02,
            Lay =                      0x03,
            Bike =                     0x04,
            Car =                      0x05,
            Horse =                    0x06,
            Row1Oar =                  0x07,
            Row2Oar =                  0x08,
            SitVReel =                 0x09,
            SitHarness =               0x0A,
            SitBar =                   0x0B,
            Observation =              0x0C,
            SitHarnessFeetDangle =     0x0D,
            HorseSteeple =             0x0E,
            Pedalo =                   0x0F,
            PedalBike =                0x10,
            Canoe =                    0x11,
            Dinghy =                   0x12,
            LayFront =                 0x13,
            RowLOar =                  0x14,
            RowROar =                  0x15,
            HorseMerryGoRound =        0x16,
            HorseSteeplechase =        0x17,
            Jump =                     0x18, // or 0x19
            BumBounce =                0x19, // or 0x18
            Dance =                    0x1A,
            SurfBoard =                0x1B,
            WindSurf =                 0x1C,
            Fishing =                  0x1D,
            SlideBodyBack =            0x1E,
            SlideRing =                0x1F,
            Inflatable =               0x20,
            SitStadium =               0x21,
            Swim =                     0x22
        };
    };
};

struct RideOptionOption {
    unsigned short suboption;
    unsigned short group;       ///< Seen 0x0000, 0x0100, 0x0200
}

struct RideOption {
    unsigned long type;
    union {
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
            float unk5;
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


#endif // SHAREDRIDE_H_INCLUDED
