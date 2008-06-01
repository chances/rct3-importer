#ifndef RCT3CONSTANTS_H_INCLUDED
#define RCT3CONSTANTS_H_INCLUDED

namespace r3 {

struct Constants {
    struct Addon {
        enum {
            Vanilla =                   0x0000,
            Soaked =                    0x0001,
            Wild =                      0x0002,
            Soaked_Hi =                 0x0200,
            Wild_Hi =                   0x0300,
        };
    };

    struct Attraction {
        struct Type {
            enum {
                Ride_Transport_Standard =    0,
                Ride_Gentle =                1,
                Ride_Water =                 2,
                Ride_Coaster_Steel =         3,
                Ride_Thrill =                4,
                Ride_Other =                 5,
                Ride_Junior =                6,
                Stall_Food =                 7,
                Stall_Drink =                8,
                Stall_Misc =                 9,
                Special_Toilet =            11,
                Special_ATM =               12,
                Special_1st_Aid =           13,
                Stall_Facility =            14,
                Ride_Coaster_Wooden =       15,
                Ride_Coaster_Suspended =    16,
                Ride_Coaster_Water =        17,
                Changing_Room =             18,
                Ride_Pool =                 19,
                Viewing_Gallery =           20,
                Ride_Transport_Safari =     21
            };
        };
        struct BaseUpkeep {
            enum {
                Some_Flats =                 0,
                Some_Coasters_A =           40, ///< Inverted Hairpin, Spinnig Steel and StandUp Coaster
                Some_Coasters_B =          125, ///< Drifting, Endless and Floorless
                Stall =                   1500,
                Scenery_Deprecated =      2500, ///< Used in Scenery...ovl, which is probabry no longer used
                Other =                   4960  ///< Other and Special attractions inside the individual ovls
            };
        };
        struct Flags {
            enum {
                No_Kids =                   0x00000001, ///< Seems to indicate that small kids cannot ride
                Unknown_2 =                 0x00000002,
                Unknown_3 =                 0x00000004, ///< Related to some water coasters and pool rides
                                                        /**<
                                                         * Set for Log Flume, Rafts, Rapids, Splash Boats,
                                                         * White Water Rapids and Lasy River
                                                         **/
                Unknown_4 =                 0x00000008, ///< Related to some water coasters
                                                        /**<
                                                         * Set only on Log Flume, Rafts, Rapids and Splash Boats
                                                         **/
                No_Testing =                0x00000010, ///< If set, the attraction doesn't support testing
                RotoDrop =                  0x00000020, ///< Set only on Roto Drop
                Unknown_7 =                 0x00000040, ///< Related to some water rides.
                                                        /**<
                                                         * Set only for Bumper Boats, Jet Skies, Swan Boats,
                                                         * Windsurfer and Master Blaster
                                                         **/
                Full_Load_Only =            0x00000080, ///< If set, only full load possible
                GiantSlide =                0x00000100, ///< Only set for giant slide
                Unknown_10 =                0x00000200, ///< Only set on BallCoaster, Rotating Tower Coaster and Seizmic
                Unknown_11 =                0x00000400, ///< Only set on the Animal Houses (Insect, Nocturnal, Reptile) and Sinking Ship
                WhiteWaterRapids =          0x00000800, ///< Only set on White Water Rapids
                Pool_Ride =                 0x00001000, ///< Set only on all pool rides.
                SlidesA =                   0x00002000, ///< Set on all pool rides with a station (ie not on Sinking Ship and Lazy River)
                Unknown_15 =                0x00004000, ///< Set on Quad Bikes, Insect House and Nocturnal House
                SlidesB =                   0x00008000, ///< Set on all pool rides with a station (ie not on Sinking Ship and Lazy River)
                SuspendedSwinging =         0x00010000, ///< Only set on Suspended Swinging Coaster
                Unknown_18 =                0x00020000, ///< Only set on Frequent Faller, Suspended Flying, Lay Down and Seizmic Coasters
            };
        };
        struct Unknown12 {
            enum {
                Default =                   0,
                Generic_Stall =             2  ///< Also used for Doughnut and French Fries
            };
        };
        struct Unknown13 {
            enum {
                Default =                   0,
                Animal_House_Ride =    606208  ///< Insect, Reptile and Nocturnal House
            };
        };
    };

    struct CarriedItem {
        struct Flag {
            // Carrieditem flags
            enum {
                NotSold =           0x00000000,  ///< None of the other < 0x1000
                                                 /**< Used in Autograph, Camera, Camcorder, GolfBall,
                                                  * GolfClub + KidGolfClub
                                                  **/
                Food =              0x00000001,
                Drink =             0x00000002,  ///< Icecream is 0x3
                Used =              0x00000004,  ///< Used up version of something
                Map =               0x00000008,
                Voucher =           0x00000010,
                Umbrella_Open =     0x00000020,  ///< Open umbrella has 0x60
                Umbrella =          0x00000040,
                Baloon =            0x00000080,  ///< Ballon is 0x280
                Souvenir =          0x00000200,
                Kiddie_Version =    0x00001000,  ///< Seen for umbrella
                Camera =            0x00002000,
                Baloon2 =           0x00004000,  ///< Set in Baloon + Sunglasses, maybe recolorable
                Camcorder =         0x00008000,
                Sunblock =          0x00010000,
                Inflatable =        0x00020000,
                Swimsuit =          0x00080000
            };
        };
        struct Type {
            // Carreiditem types, probably flags as well
            enum {
                Invisible =             1,
                Hand =                  2,
                Body =                  4, ///< Shirts, SunBlocker
                Two_Hands =             6, ///< Maybe body or hand
                Head =                  8,
                Sunglasses =           16,
                Swimsuit =             36
            };
        };

    };

    struct FlexiTexture {
        struct Recolourable {
            enum {
                First =         1,
                Second =        2,
                Third =         4
            };
        };
        typedef Recolourable Recolorable;
    };

    struct Mesh {
        struct SupportType {
            enum {
                Below =         1,
                Top =           2,
                Mid =           4,
                GroundAttach =  8,
                None =          0xFFFFFFFF
            };
        };
        struct Transparency {
            enum {
                None =          0,
                Simple =        1,  ///< Used with Mask texture styles
                Complex =       2   ///< used with full trasparency styles
            };
        };
        struct Flags {
            enum {
                None =                      0,
                ScrollingSign =            12,
                WaterWheelChain =          20,
                Wheels1 =                  36,
                Wheels2 =                  68,
                Terrain =               12288,
                Cliff =                 20480,
                AnimatedBillboard =     32768,
                Billboard =             32788
            };
        };
        struct Sides {
            enum {
                Doublesided = 1,
                Singlesided = 3
            };
        };
    };

    struct SharedRide {
        /// Seating enum
        struct Seating {
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
                Sit_Harness_Feet_Dangle =  0x0D,
                Horse_Steeple =            0x0E,
                Pedalo =                   0x0F,
                Pedal_Bike =               0x10,
                Canoe =                    0x11,
                Dinghy =                   0x12,
                Lay_Front =                0x13,
                Row_L_Oar =                0x14,
                Row_R_Oar =                0x15,
                Horse_Merry_Go_Round =     0x16,
                Horse_Steeplechase =       0x17,
                Jump =                     0x18, // or 0x19
                Bum_Bounce =               0x19, // or 0x18
                Dance =                    0x1A,
                Surf_Board =               0x1B,
                Wind_Surf =                0x1C,
                Fishing =                  0x1D,
                Slide_Body_Back =          0x1E,
                Slide_Ring =               0x1F,
                Inflatable =               0x20,
                Sit_Stadium =              0x21,
                Swim =                     0x22
            };
        };
        struct Option_Application_Type {
            enum {
                Base =                      0x00,
                Circuits =                  0x02,
                Car_Count =                 0x03,
                Ride_Duration =             0x04,
                Scenery_Max =               0x05, ///< Maximal scenery influence
                Train_Count =               0x06,
                Train_Type_Order =          0x07, ///< Influence per index in the train tye list
                Launch_Speed =              0x09,
                Slow_Lift_Chain =           0x0A, ///< Applied if the chain speed is very slow
                Lift_Chain_Speed =          0x0B,
                Speed_Max =                 0x0D,
                Speed_Mean =                0x0E,
                Ride_Length =               0x0F,
                Max_Positive_V_GForce =     0x10,
                Max_Negative_V_GForce =     0x11,
                Max_Lateral_GForce =        0x12,
                Airtime =                   0x13,
                Drops =                     0x14,
                Max_Drop_Height =           0x15,
                Inversions =                0x16,
                Tunnel_Length =             0x18,
                Terraforming =              0x19,
                Scenery_Influence =         0x20, ///< Amount of scenery influence
                Lateral_GForce_Mean =       0x22,
                Vertical_GForce_Mean =      0x23
            };
        };
    };

    struct SVD {
        struct LOD_Type {
            enum {
                Static =     0,
                Animated =   3,
                Billboard =  4
            };
        };
        struct Flags {
            enum {
                Greenery =              0x00000001,
                Flower =                0x00000002,
                No_Shadow =             0x00000002,
                Rotation =              0x00000004,
                Unknown =               0x00000070,
                Soaked =                0x01000000,
                Wild =                  0x02000000
            };
        };
    };

    struct SID {
        struct Position {
            enum {
                Tile_Full =                 0,
                Path_Edge_Inner =           1,
                Path_Edge_Outer =           2,
                Wall =                      3,
                Tile_Quarter =              4,
                Tile_Half =                 5,
                Path_Center =               6,
                Corner =                    7,
                Path_Edge_Join =            8
            };
        };
        struct Type {
            enum {
                Tree =                       0,
                Plant =                      1,
                Shrub =                      2,
                Flowers =                    3,
                Fence =                      4,
                Wall_Misc =                  5,
                Path_Lamp =                  6,
                Scenery_Small =              7,
                Scenery_Medium =             8,
                Scenery_Large =              9,
                Scenery_Anamatronic =       10,
                Scenery_Misc =              11,
                Support_Middle =            12,
                Support_Top =               13,
                Support_Bottom =            14,
                Support_Bottom_Extra =      15,
                Support_Girder =            16,
                Support_Cap =               17,
                Ride_Track =                18,
                Path =                      19,
                Park_Entrance =             20,
                Litter =                    21,
                Guest_Inject =              22,
                Ride =                      23,
                Ride_Entrance =             24,
                Ride_Exit =                 25,
                Keep_Clear_Fence =          26,
                Stall =                     27,
                Ride_Event =                28,
                Firework =                  29,
                Litter_Bin =                30,
                Bench =                     31,
                Sign =                      32,
                Photo_Point =               33,
                Wall_Straight =             34,
                Wall_Roof =                 35,
                Wall_Corner =               36,
            //new for Soaked!
                Water_Cannon =              37,
                Pool_Piece =                38,
                Pool_Extra =                39,
                Changing_Room =             40,
                Laser_Dome =                41, //???
                Water_Jet =                 42, //???
                Terrain_Piece =             43, //???
                Particle_Effect =           44, //???
            //new for Wild!
                Animal_Fence =              45, //???
                Animal_Misc =               46  //???
            };
        };
    };

    struct TRR {
        struct Start_Preset {
            enum {
                Circle =                        0,
                Launched =                      1,
                Reverse_Tilted =                2,
                Boat =                          3,
                Race =                          4,
                Shuttle =                       5,
                Upwards =                       6,
                Freefall =                      7,
                Up_and_Down =                   8,
                Minigolf =                      9,
                Slide =                        10,
                Reverse_Cable_Lifthill =       11,
                Water_Transport =              12,
                Aquarium =                     13,
                Multiple_Powerlaunch =         14,
                Lazy_River =                   15
            };
        };
        struct Start_Possibilities {
            enum {
                Circle =                        0x00000001,
                Launched =                      0x00000002,
                Reverse_Tilted =                0x00000004,
                Boat =                          0x00000008,
                Race =                          0x00000010,
                Shuttle =                       0x00000020,
                Upwards =                       0x00000040,
                Freefall =                      0x00000080,
                Up_and_Down =                   0x00000100,
                Minigolf =                      0x00000200,
                Slide =                         0x00000400,
                Reverse_Cable_Lifthill =        0x00000800,
                Water_Transport =               0x00001000,
                Aquarium =                      0x00002000,
                Multiple_Powerlaunch =          0x00004000,
                Lazy_River =                    0x00008000
            };
        };
    };
};

};

#endif // RCT3CONSTANTS_H_INCLUDED
