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
        struct UI_Deactivation {
            enum {
                None =                      0x00000000,
                Flag =                      0x00000001, ///< Open/Test/Close Flag
                Details =                   0x00000002,
                Guest_Thoughts =            0x00000004,
                Finances =                  0x00000008,
                Customers =                 0x00000010,
                Unknown_1 =                 0x00000020,
                Unknown_2 =                 0x00000040,
                Unknown_3 =                 0x00000080,
                Unknown_4 =                 0x00000100,
                Unknown_5 =                 0x00000200,
                Unknown_6 =                 0x00000400,
                Unknown_7 =                 0x00000800,
                Unknown_8 =                 0x00001000,
                Unknown_9 =                 0x00002000,
                Vehicles =                  0x00004000,
                Test_Results =              0x00008000,
                Operating_Mode =            0x00010000,
                Maintenance =               0x00020000,
                Construction =              0x00040000, ///< also deactivated (without this flag) on animal houses
                Graphs =                    0x00080000,
                Unknown_10 =                0x00100000,
                Animal_House_Ride =         0x00094000  ///< Insect, Reptile and Nocturnal House
                // Unknowns had no effect on tracked rides, more than 0x00100000 was not tested
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
                None =                  0x00000000, //    0,
                ScrollingSign =         0x0000000C, //   12,
                WaterWheelChain =       0x00000014, //   20,
                Wheels1 =               0x00000024, //   36,
                Wheels2 =               0x00000044, //   68,
                Terrain =               0x00003000, //12288,
                Cliff =                 0x00005000, //20480,
                AnimatedBillboard =     0x00008000, //32768,
                Billboard =             0x00008014, //32788
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
                Row1Oar =                  0x07, // Not used in full extended state
                Row2Oar =                  0x08, // Not used in full extended state
                SitVReel =                 0x09,
                SitHarness =               0x0A,
                SitBar =                   0x0B,
                Observation =              0x0C,
                Sit_Harness_Feet_Dangle =  0x0D,
                Horse_Steeple =            0x0E,
                Pedalo =                   0x0F,
                Pedal_Bike =               0x10, // Not used in full extended state
                Canoe =                    0x11,
                Dinghy =                   0x12,
                Lay_Front =                0x13,
                Row_L_Oar =                0x14,
                Row_R_Oar =                0x15,
                Horse_Merry_Go_Round =     0x16,
                Horse_Steeplechase =       0x17,
                Bum_Bounce =               0x18, // or 0x19, Not directly used
                Jump =                     0x19, // or 0x18
                Dance =                    0x1A,
                MAX_V =                    0x1B,
                Surf_Board =               0x1B,
                Wind_Surf =                0x1C,
                Fishing =                  0x1D, // Not used in full extended state
                Slide_Body_Back =          0x1E,
                Slide_Ring =               0x1F,
                Inflatable =               0x20, // Not used in anr/ric in full extended state
                Sit_Stadium =              0x21,
                Swim =                     0x22,
                MAX_SW =                   0x23
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
        struct Station_Limit_Flags {
            enum {
                To_Minus_X =                0x00000001,
                To_Plus_Z =                 0x00000002,
                To_Plus_X =                 0x00000004,
                To_Minus_Z =                0x00000008,
                Entrance =                  0x00000010,
                Exit =                      0x00000020,
                Flip01 =                    0x00000040,
                Flip02 =                    0x00000100,
                Slide_End =                 0x00000200,
                Slide_End_to_Lifthill =     0x00000400  ///< 400 alone is between slide end and lift hill. 600 where hill/slide ends
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
                Greenery =              0x00000001, ///< Set on Trees, Shrubs & Fern, also on lots of walls
                No_Shadow =             0x00000002,
                Flower =                0x00000002,
                Rotation =              0x00000004,
                Unknown01 =             0x00000010,
                Unknown02 =             0x00000020,
                Unknown03 =             0x00000040,
                Animated_Preview =      0x00000800,
                Unknown_Giant_Ferris =  0x00001000,
                Soaked =                0x01000000,
                Wild =                  0x02000000,
                Soaked_or_Wild =        0x03000000
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
        struct Flags {
            enum {
                Unknown_01 =            0x00000001,
                Unknown_02 =            0x00000002, ///< Never set?
                Unknown_03 =            0x00000004,
                Unknown_04 =            0x00000008,
                Delete_On_Ground_Change=0x00000010,
                Ground_Change =         0x00000020, ///< Goes with ground if non-colliding, blocks ground change otherwise
                Unknown_07 =            0x00000040,
                Unknown_08 =            0x00000080,
                Support_Block =         0x00000100, ///< Blocks supports (from above only, may require collision detection
                Unknown_10 =            0x00000200, ///< Set on track pieces (seen on Giant Flume)
                Unknown_11 =            0x00000400, ///< Never set?
                Unknown_12 =            0x00000800, ///< Never set?
                Skew =                  0x00001000, ///< Object skews with ground (like fences). Might depend on other things
                Smooth_Height =         0x00002000, ///< Smooth height change, not in increments.
                Unknown_15 =            0x00004000, ///< Never set?
                Unknown_16 =            0x00008000, ///< Never set?
                Water_Only =            0x00010000, ///< Removes base of flats and makes them placable on water only.
                Unknown_18 =            0x00020000, ///< Never set?
                Unknown_19 =            0x00040000, ///< Never set?
                Unknown_20 =            0x00080000, ///< Never set?
                Unknown_21 =            0x00100000, ///< Never set?
                Exact_Fence =           0x00200000, ///< On rides, fence only around parts touching the floor (collision detection)
                Unknown_23 =            0x00400000, ///< Never set?
                Fences =                0x00800000, ///< Set for fences and fence-like objects. Purpose unknown.
                Unknown_25 =            0x01000000, ///< Never set?
                Billboard_Menu =        0x02000000, ///< Puts object in billboard menu
                Unknown_27 =            0x04000000, ///< Never set?
                Unknown_28 =            0x08000000, ///< Never set?
                Unknown_29 =            0x10000000, ///< Never set?
                Unknown_30 =            0x20000000, ///< Never set?
                Unknown_31 =            0x40000000, ///< Never set?
                Unknown_32 =            0x80000000, ///< Never set?
            };
        };
        struct Square_Flags {
            enum {
                Collision =             0x00000001, ///< Activates collision detection
                Supports =              0x00000002, ///< Show supports. Needs Collision and the respective setting
                Unknown_35 =            0x00000004, ///< Never set?
                Unknown_36 =            0x00000008,
                Unknown_37 =            0x00000010,
                Unknown_38 =            0x00000020,
                Unknown_39 =            0x00000040,
                Unknown_40 =            0x00000080,
                Unknown_41 =            0x00000100,
                Unknown_42 =            0x00000200,
                Unknown_43 =            0x00000400,
                Unknown_44 =            0x00000800
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
        struct Group_Flags {
            enum {
                Type_Group_Name =               0x00000000,
                Type_Track_Ref =                0x00000001,
                Type_Nothing =                  0x00000002,
                Unknown_01 =                    0x00000100, ///< Condition: Not there, absence imlpcitily means There
                Unknown_02 =                    0x00000200, ///< Only on ring slide
                Unknown_03 =                    0x00000400, ///< Only on ring slide
                Unknown_04 =                    0x00000800, ///< If we want to build backwards A, preview B
                Unknown_05 =                    0x00001000  ///< If we want to build forwards A, preview B
            };
        };
    };
    struct TKS {
        struct Special_Part {
            enum {
                None =                           0,
                Breakes =                        1,
                Block_Breakes =                  2,
                Station =                        3,
                Photosection =                   4,
                S_Curve =                        5,
                 Banked_Rise_Left_45_CT9 =       5,
                S_Curve_Tilted =                 6,
                Corkscrew =                      7,
                 Half_Loop_Inv_Small_33 =        7,
                 Banked_Rise_Right_45_CT9 =      7,
                 S_Curve_to_S_Curve_Wide_TB25 =  7,
                Corkscrew_Large =                8,
                 S_Curve_Wide_to_S_Curve_TB25 =  8,
                Half_Loop =                      9,
                Quarter_Loop =                  10,
                Half_Loop_Large =               11,
                Loop =                          12,
                Figure_8_Loop =                 13,
                Inline_Twist =                  14,
                 Vertical_Chain_Left =          14,
                 Zero_G_Roll_Medium_45_Right_CT9 = 14,
                Barrel_Roll =                   15,
                 Vertical_Chain =               15,
                Overbanked_Turn_Small =         16,
                Overbanked_Turn_Large =         17,
                Overbanked_Turn_Medslope =      18,
                Helix_Up =                      19,
                 Half_Loop_Down_Medium_33 =     19,
                 Overbanked_Turn_Small_Right_34 = 19,
                Helix_Down =                    20,
                 Half_Loop_Down_Large_33 =      20,
                Helix_Up_Small =                21,
                Helix_Up_Large =                22,
                Quarter_Helix =                 23, // Inverted?
                Holding_Break =                 24,
                Lift_Hill_Curved =              25,
                Spinning_Control_Toggle =       26,
                Reverser =                      27,
                Lift_Hill_Launched =            28,
                Lift_Hill_Cable =               29,
                Transfer_Upper =                30,
                Transfer_Lower =                31,
                Lift_End =                      32, // Top and Bottom
                Tilt_Section =                  33,
                Drop_95_Degrees =               34,
                Spinning_Tunnel =               36,
                Log_Reverser =                  37,
                Log_Bumps =                     38,
                Water_Splash =                  39,
                Rapids =                        40,
                Waterfall =                     41,
                 Launch_Section_CT9 =           41,
                Whirlpool =                     42,
                Zero_G_Roll_Steep =             43,
                 Zero_G_Roll_Medium_45_Left_CT5=43,
                Zero_G_Roll_Medium =            44,
                Lift_Middle =                   47, // Also Elevator, Giant Slide Middle & Top
                Diving_Loop_90_Left =           50,
                 Golf_Hole_01 =                 50,
                Diving_Loop_90_Right =          51,
                 Golf_Hole_02 =                 51,
                Golf_Hole_03 =                  52,
                Golf_Hole_04 =                  53,
                Golf_Hole_05 =                  54,
                Golf_Hole_06 =                  55,
                Vertical_Tight_Curve =          59,
                Steep_Slope_to_Vertical =       60,
                Vertical_to_Steep_Slope =       61,
                Serpent_Head =                  62,
                Serpent_Tail =                  63,
                Helix_Down_Small =              64,
                Helix_Down_Large =              65,
                Water_Spray =                   66,
                Inv_Station_Middle_End =        67, // Track27
                Aquarium_Arch =                 68,
                Aquarium_Circle =               69,
                Aquarium_Inside_L =             70,
                Aquarium_Inside_Wall =          71,
                Aquarium_Ray =                  73,
                Spiral_Lift_Hill_Bottom =       74,
                Spiral_Lift_Hill_Middle =       75,
                Spiral_Lift_Hill_Top =          76,
                Rapids_White_Water_01 =         77,
                Rapids_White_Water_02 =         78,
                Straight_to_Vertical_Small =    80,
                Straight_to_Vertical_Medium =   81,
                Slide_Station_Transition =      82,
                Slide_Bowl =                    83,
                Slide_Funnel =                  84,
                Slide_End =                     85,
                Slide_End_to_Lift_Hill =        86,
                Flying_Snake_Dive =             88,
                Diving_Loop =                   89,
                Corkscrew_Loose =               90,
                Rotating_Tower_Base =           91,
                Rotating_Tower_Vartical =       92,
                Rotating_Tower_Cap_180 =        93,
                Rotating_Tower_Cap_90_CW =      94,
                Loop_Tilt_Incline =             96,
                Straight_to_Vertical_Large =    97,
                Inv_Straight_to_Vertical_Small =99,
                Vertical_to_Straight_Large =            100,
                Vertical_to_Straight_Small =            101,
                Vertical_to_Inv_Straight_Large =        102,
                Vertical_to_Inv_Straight_Small =        103,
                Steep_Slope_to_Straight_Short =         104,
                Steep_Slope_to_Straight_Short_Chain =   105,
                Straight_to_Steep_Slope_Short =         106,
                Straight_to_Steep_Slope_Short_Chain =   107,
                Half_Loop_Large_33 =                    108,
                Half_Loop_Medium_33 =                   109,
                Half_Loop_Small_33 =                    110,
                Half_Loop_Down_Small_33 =               111,
                Half_Loop_Inv_Large_33 =                112,
                Half_Loop_Inv_Down_Large_33 =           113,
                Half_Loop_Inv_Medium_33 =               114,
                Half_Loop_Inv_Down_Medium_33 =          115,
                Half_Loop_Inv_Down_Small_33 =           116,
                Straight_to_Vertical_Down_Inv_Large =   117,
                Vertical_to_Straight_Inv_Large =        118,
                Straight_to_Vertical_Down_Large =       119,
                Vertical_to_Straight_Down_Large =       120,
                Loop_Large_Left =                       121,
                Loop_Large_Right =                      122,
                Loop_Small_Left =                       123,
                Loop_Small_Right =                      124,
                Overbanked_Turn_Small_Left_34 =         125,
                Vertical_to_Inv_Straight_Down_Large =   126,
                Inv_Corkscrew =                         127, // Tr5
                Inv_Corkscrew_Large =                   128  // Tr5
            };
        };
        struct Direction {
            enum {
                Straight =                              0,
                Left =                                  1,
                Right =                                 2
            };
        };
        struct Flags {
            enum {
                Start_Section =                     0x00000001, ///< Player can start building with this section
                Chain =                             0x00000002,
                Forward_Only =                      0x00000004, ///< If not set, the part can be built in or against spline direction. If set, only in spline direction.
                Capped =                            0x00000008, ///< Capped version. Activates UI element.
                Transfer =                          0x00000010, ///< Heartline coaster, reverse at mid of spline
                Cable_Lift =                        0x00000020,
                Station =                           0x00000040,
                Tower_Ride_Base =                   0x00000080,
                Hold =                              0x00000100, ///< Train is held for X seconds. Enables respective animations
                Lift_Bottom =                       0x00000200,
                Lift_Middle =                       0x00000400,
                Lift_Top =                          0x00000800,
                Water_Option =                      0x00001000,
                Station_End_Suspended_Mono =        0x00002000,
                Accelerating =                      0x00004000, ///< Launch sections
                Rapids_Tight_Curve =                0x00008000,
                Reverser =                          0x00010000, ///< Extra spline turns car
                Alternate_Spline =                  0x00020000, ///< Extra spline is alternate car spline
                Trigger_Alternate_Spline =          0x00040000, ///< Triggers alternate spline (see previous) in next section
                Unknown_Special =                   0x00200000, ///< Usually but not allways on capped versions. Also on lifts. Cannot fall off?
                Extended_Structure =                0x00400000, ///< Structure is Soaked/Wild extended
                Serpent_Head =                      0x00800000, ///< Maybe cable lift house?
                Booster =                           0x01000000, ///< Aqua Blaster
                Aquarium =                          0x02000000,
                Ring_Slide_Station_1 =              0x08000000,
                Ring_Slide_Station_2 =              0x20000000
            };
        };
        struct Slope {
            enum {
                Flat =          0,
                Medium_Up =     1,
                Medium_Down =   2,
                Steep_Up =      3,
                Steep_Down =    4,
                Vertical_Up =   5,
                Vertical_Down = 6,
                Auto_End =      7       ///< Only used for chair lift end
            };
        };
        struct Bank {
            enum {
                Flat =          0,
                Half_Left =     1,
                Left =          2,
                Inv_Half_Left = 3,
                Inv =           4,
                Inv_Half_Right= 5,
                Right =         6,
                Half_Right =    7
            };
        };
        struct Splitter_Half {
            enum {
                Not_Splittable = 0,
                Left           = 1,
                Right          = 2
            };
        };
        struct Rotator_Type {
            enum {
                None =          0,
                Rotate_180 =    1,
                Rotate_90CW =   2
            };
        };
    };
};

};

#endif // RCT3CONSTANTS_H_INCLUDED
