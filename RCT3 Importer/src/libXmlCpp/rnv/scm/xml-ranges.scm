; $Id: xml-ranges.scm 345 2004-01-25 09:44:04Z dvd $

(define xml-ranges '(
  (base-char . #((#x41 . #x5a) (#x61 . #x7a) (#xc0 . #xd6)
    (#xd8 . #xf6) (#xf8 . #xff) (#x100 . #x131) (#x134 . #x13e)
    (#x141 . #x148) (#x14a . #x17e) (#x180 . #x1c3) (#x1cd . #x1f0)
    (#x1f4 . #x1f5) (#x1fa . #x217) (#x250 . #x2a8) (#x2bb . #x2c1)
    (#x386 . #x386) (#x388 . #x38a) (#x38c . #x38c) (#x38e . #x3a1)
    (#x3a3 . #x3ce) (#x3d0 . #x3d6) (#x3da . #x3da) (#x3dc . #x3dc)
    (#x3de . #x3de) (#x3e0 . #x3e0) (#x3e2 . #x3f3) (#x401 . #x40c)
    (#x40e . #x44f) (#x451 . #x45c) (#x45e . #x481) (#x490 . #x4c4)
    (#x4c7 . #x4c8) (#x4cb . #x4cc) (#x4d0 . #x4eb) (#x4ee . #x4f5)
    (#x4f8 . #x4f9) (#x531 . #x556) (#x559 . #x559) (#x561 . #x586)
    (#x5d0 . #x5ea) (#x5f0 . #x5f2) (#x621 . #x63a) (#x641 . #x64a)
    (#x671 . #x6b7) (#x6ba . #x6be) (#x6c0 . #x6ce) (#x6d0 . #x6d3)
    (#x6d5 . #x6d5) (#x6e5 . #x6e6) (#x905 . #x939) (#x93d . #x93d)
    (#x958 . #x961) (#x985 . #x98c) (#x98f . #x990) (#x993 . #x9a8)
    (#x9aa . #x9b0) (#x9b2 . #x9b2) (#x9b6 . #x9b9) (#x9dc . #x9dd)
    (#x9df . #x9e1) (#x9f0 . #x9f1) (#xa05 . #xa0a) (#xa0f . #xa10)
    (#xa13 . #xa28) (#xa2a . #xa30) (#xa32 . #xa33) (#xa35 . #xa36)
    (#xa38 . #xa39) (#xa59 . #xa5c) (#xa5e . #xa5e) (#xa72 . #xa74)
    (#xa85 . #xa8b) (#xa8d . #xa8d) (#xa8f . #xa91) (#xa93 . #xaa8)
    (#xaaa . #xab0) (#xab2 . #xab3) (#xab5 . #xab9) (#xabd . #xabd)
    (#xae0 . #xae0) (#xb05 . #xb0c) (#xb0f . #xb10) (#xb13 . #xb28)
    (#xb2a . #xb30) (#xb32 . #xb33) (#xb36 . #xb39) (#xb3d . #xb3d)
    (#xb5c . #xb5d) (#xb5f . #xb61) (#xb85 . #xb8a) (#xb8e . #xb90)
    (#xb92 . #xb95) (#xb99 . #xb9a) (#xb9c . #xb9c) (#xb9e . #xb9f)
    (#xba3 . #xba4) (#xba8 . #xbaa) (#xbae . #xbb5) (#xbb7 . #xbb9)
    (#xc05 . #xc0c) (#xc0e . #xc10) (#xc12 . #xc28) (#xc2a . #xc33)
    (#xc35 . #xc39) (#xc60 . #xc61) (#xc85 . #xc8c) (#xc8e . #xc90)
    (#xc92 . #xca8) (#xcaa . #xcb3) (#xcb5 . #xcb9) (#xcde . #xcde)
    (#xce0 . #xce1) (#xd05 . #xd0c) (#xd0e . #xd10) (#xd12 . #xd28)
    (#xd2a . #xd39) (#xd60 . #xd61) (#xe01 . #xe2e) (#xe30 . #xe30)
    (#xe32 . #xe33) (#xe40 . #xe45) (#xe81 . #xe82) (#xe84 . #xe84)
    (#xe87 . #xe88) (#xe8a . #xe8a) (#xe8d . #xe8d) (#xe94 . #xe97)
    (#xe99 . #xe9f) (#xea1 . #xea3) (#xea5 . #xea5) (#xea7 . #xea7)
    (#xeaa . #xeab) (#xead . #xeae) (#xeb0 . #xeb0) (#xeb2 . #xeb3)
    (#xebd . #xebd) (#xec0 . #xec4) (#xf40 . #xf47) (#xf49 . #xf69)
    (#x10a0 . #x10c5) (#x10d0 . #x10f6) (#x1100 . #x1100) (#x1102 . #x1103)
    (#x1105 . #x1107) (#x1109 . #x1109) (#x110b . #x110c) (#x110e . #x1112)
    (#x113c . #x113c) (#x113e . #x113e) (#x1140 . #x1140) (#x114c . #x114c)
    (#x114e . #x114e) (#x1150 . #x1150) (#x1154 . #x1155) (#x1159 . #x1159)
    (#x115f . #x1161) (#x1163 . #x1163) (#x1165 . #x1165) (#x1167 . #x1167)
    (#x1169 . #x1169) (#x116d . #x116e) (#x1172 . #x1173) (#x1175 . #x1175)
    (#x119e . #x119e) (#x11a8 . #x11a8) (#x11ab . #x11ab) (#x11ae . #x11af)
    (#x11b7 . #x11b8) (#x11ba . #x11ba) (#x11bc . #x11c2) (#x11eb . #x11eb)
    (#x11f0 . #x11f0) (#x11f9 . #x11f9) (#x1e00 . #x1e9b) (#x1ea0 . #x1ef9)
    (#x1f00 . #x1f15) (#x1f18 . #x1f1d) (#x1f20 . #x1f45) (#x1f48 . #x1f4d)
    (#x1f50 . #x1f57) (#x1f59 . #x1f59) (#x1f5b . #x1f5b) (#x1f5d . #x1f5d)
    (#x1f5f . #x1f7d) (#x1f80 . #x1fb4) (#x1fb6 . #x1fbc) (#x1fbe . #x1fbe)
    (#x1fc2 . #x1fc4) (#x1fc6 . #x1fcc) (#x1fd0 . #x1fd3) (#x1fd6 . #x1fdb)
    (#x1fe0 . #x1fec) (#x1ff2 . #x1ff4) (#x1ff6 . #x1ffc) (#x2126 . #x2126)
    (#x212a . #x212b) (#x212e . #x212e) (#x2180 . #x2182) (#x3041 . #x3094)
    (#x30a1 . #x30fa) (#x3105 . #x312c) (#xac00 . #xd7a3)))

  (ideographic . #((#x3007 . #x3007) (#x3021 . #x3029) (#x4e00 . #x9fa5)))

  (combining-char . #((#x300 . #x345) (#x360 . #x361) (#x483 . #x486)
    (#x591 . #x5a1) (#x5a3 . #x5b9) (#x5bb . #x5bd) (#x5bf . #x5bf)
    (#x5c1 . #x5c2) (#x5c4 . #x5c4) (#x64b . #x652) (#x670 . #x670)
    (#x6d6 . #x6dc) (#x6dd . #x6df) (#x6e0 . #x6e4) (#x6e7 . #x6e8)
    (#x6ea . #x6ed) (#x901 . #x903) (#x93c . #x93c) (#x93e . #x94c)
    (#x94d . #x94d) (#x951 . #x954) (#x962 . #x963) (#x981 . #x983)
    (#x9bc . #x9bc) (#x9be . #x9be) (#x9bf . #x9bf) (#x9c0 . #x9c4)
    (#x9c7 . #x9c8) (#x9cb . #x9cd) (#x9d7 . #x9d7) (#x9e2 . #x9e3)
    (#xa02 . #xa02) (#xa3c . #xa3c) (#xa3e . #xa3e) (#xa3f . #xa3f)
    (#xa40 . #xa42) (#xa47 . #xa48) (#xa4b . #xa4d) (#xa70 . #xa71)
    (#xa81 . #xa83) (#xabc . #xabc) (#xabe . #xac5) (#xac7 . #xac9)
    (#xacb . #xacd) (#xb01 . #xb03) (#xb3c . #xb3c) (#xb3e . #xb43)
    (#xb47 . #xb48) (#xb4b . #xb4d) (#xb56 . #xb57) (#xb82 . #xb83)
    (#xbbe . #xbc2) (#xbc6 . #xbc8) (#xbca . #xbcd) (#xbd7 . #xbd7)
    (#xc01 . #xc03) (#xc3e . #xc44) (#xc46 . #xc48) (#xc4a . #xc4d)
    (#xc55 . #xc56) (#xc82 . #xc83) (#xcbe . #xcc4) (#xcc6 . #xcc8)
    (#xcca . #xccd) (#xcd5 . #xcd6) (#xd02 . #xd03) (#xd3e . #xd43)
    (#xd46 . #xd48) (#xd4a . #xd4d) (#xd57 . #xd57) (#xe31 . #xe31)
    (#xe34 . #xe3a) (#xe47 . #xe4e) (#xeb1 . #xeb1) (#xeb4 . #xeb9)
    (#xebb . #xebc) (#xec8 . #xecd) (#xf18 . #xf19) (#xf35 . #xf35)
    (#xf37 . #xf37) (#xf39 . #xf39) (#xf3e . #xf3e) (#xf3f . #xf3f)
    (#xf71 . #xf84) (#xf86 . #xf8b) (#xf90 . #xf95) (#xf97 . #xf97)
    (#xf99 . #xfad) (#xfb1 . #xfb7) (#xfb9 . #xfb9) (#x20d0 . #x20dc)
    (#x20e1 . #x20e1) (#x302a . #x302f) (#x3099 . #x3099)
    (#x309a . #x309a)))

  (digit . #((#x30 . #x39) (#x660 . #x669) (#x6f0 . #x6f9) (#x966 . #x96f)
    (#x9e6 . #x9ef) (#xa66 . #xa6f) (#xae6 . #xaef) (#xb66 . #xb6f)
    (#xbe7 . #xbef) (#xc66 . #xc6f) (#xce6 . #xcef) (#xd66 . #xd6f)
    (#xe50 . #xe59) (#xed0 . #xed9) (#xf20 . #xf29)))

  (extender . #((#xb7 . #xb7) (#x2d0 . #x2d1) (#x387 . #x387)
    (#x640 . #x640) (#xe46 . #xe46) (#xec6 . #xec6) (#x3005 . #x3005)
    (#x3031 . #x3035) (#x309d . #x309e) (#x30fc . #x30fe)))))
