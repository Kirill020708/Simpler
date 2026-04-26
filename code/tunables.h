#pragma once


#define qsFPmargin 124 // 0 300
#define qsFPseeMargin -15 // -100 100

#define corrplexityMargin 103 // 0 300

#define rfpBaseD2 37 // 0 70
#define rfpImprovingD2 8 // 0 40
#define rfpCorrplexityD2 20 // 0 40
#define rfpWorseningD2 2 // 0 40
#define rfpCutnodeD2 0 // 0 40
#define rfpBaseD1 9 // 0 70
#define rfpImprovingD1 3 // 0 40
#define rfpCorrplexityD1 2 // 0 40
#define rfpWorseningD1 1 // 0 40
#define rfpCutnodeD1 3 // 0 40
#define rfpBaseD0 11 // 0 70
#define rfpImprovingD0 3 // 0 40
#define rfpCorrplexityD0 4 // 0 40
#define rfpWorseningD0 0 // 0 40
#define rfpCutnodeD0 4 // 0 40
#define rfpFail 436 // 0 1024

#define nmpBaseMargin 24 // 0 80
#define nmpDepthMargin 996 // 0 10240
#define nmpDepth2Margin 104 // 0 10240
#define nmpRbase 5190 // 0 10240 341
#define nmpRdepth 137 // 0 1024
#define nmpRimproving 121 // 0 3072
#define nmpRcutnode -69 // -4096 4096
#define nmpRmargin 198 // 50 500
#define nmpRmarginClamp 5926 // 512 10240

#define razorBaseD2 180 // 100 400
#define razorImprovingD2 27 // 0 70
#define razorBaseD1 25 // 0 300
#define razorImprovingD1 1 // 0 70
#define razorBaseD0 218 // 0 400
#define razorImprovingD0 6 // 0 70
#define razorFPmargin 23 // 0 150
#define razorRFPmargin 114 // 0 500

#define probcutBase 164 // 0 500
#define probcutImproving 43 // 0 100
#define probcutFail 997 // 0 1024

#define singextMarginDepth 890 // 0 1024
#define dextMarginBase 14834 // 0 15360
#define dextMarginHistory 10 // 0 10
#define trextMarginBase 68 // 0 80

#define lmpBaseD2 1827 // 0 4096
#define lmpImprovingD2 197 // 0 1024
#define lmpTTcaptureD2 644 // 0 1024
#define lmpBaseD1 573 // 0 4096
#define lmpImprovingD1 217 // 0 1024
#define lmpTTcaptureD1 21 // 0 1024
#define lmpBaseD0 2991 // 0 5120
#define lmpImprovingD0 132 // 0 1024
#define lmpTTcaptureD0 83 // 0 1024
#define lmpHistoryThreshold -84 // -1280 1280

#define historyBaseD2 175 // 0 800
#define historyImprovingD2 32 // 0 200
#define historyTTcaptureD2 14 // 0 200
#define historyBaseD1 177 // 0 800
#define historyImprovingD1 41 // 0 200
#define historyTTcaptureD1 90 // 0 200
#define historyBaseD0 93 // 0 800
#define historyImprovingD0 1 // 0 200
#define historyTTcaptureD0 14 // 0 200

#define fpBaseD2 76 // 0 500
#define fpImprovingD2 18 // 0 200
#define fpTTcaptureD2 90 // 0 200
#define fpHistoryD2 91 // 0 200
#define fpBaseD1 23 // 0 500
#define fpImprovingD1 4 // 0 200
#define fpTTcaptureD1 17 // 0 200
#define fpHistoryD1 16 // 0 200
#define fpBaseD0 2 // 0 500
#define fpImprovingD0 3 // 0 200
#define fpTTcaptureD0 6 // 0 200
#define fpHistoryD0 27 // 0 200

#define seeBaseD0 2 // 0 500
#define seeHistoryD0 9 // 0 200
#define seeBaseD1 96 // 0 500
#define seeHistoryD1 57 // 0 200
#define seeBaseD2 70 // 0 500
#define seeHistoryD2 4 // 0 200

#define lmrDivisor 227 // 0 2048
#define lmrBase 640 // 0 4096
#define lmrPv 365 // 0 4096
#define lmrHistory 1393 // 0 4096
#define lmrImproving 704 // 0 4096
#define lmrTTcapture 915 // 0 4096
#define lmrCutnode 962 // 0 4096
#define lmrTtpv 696 // 0 4096
#define lmrCapture 679 // 0 4096
#define lmrSee 1934 // 0 4096
#define lmrKiller 1018 // 0 4096

#define historyBonusD0 273 // -1600 1600
#define historyBonusD1 140 // 0 1600
#define historyBonusD2 301 // 0 1600
#define historyMaluseD0 1008 // -1600 1600
#define historyMaluseD1 163 // 0 1600
#define historyMaluseD2 7 // 0 1600

#define aspirationWindowSize 77 // 15 200
#define aspirationWindowMult 3155 // 1800 10240

#define bmStab0 2560 // 300 5120 *
#define bmStab1 1229 // 300 5120 *
#define bmStab2 922 // 300 5120 *
#define bmStab3 819 // 300 5120 *
#define bmStab4 768 // 300 5120 *
#define nodesTM 1741 // 1200 4096 *

#define historyScoreFromTo 1020 // 0 2048
#define historyScorePieceTo 800 // 0 2048
#define historyScorePly1 990 // 0 2048
#define historyScorePly2 837 // 0 2048
#define historyScorePly4 808 // 0 2048

#define corrhistPawn 78 // 0 1024
#define corrhistMinor 12 // 0 1024
#define corrhistColor 109 // 0 1024
#define corrhistFromTo 20 // 0 1024
#define corrhistPly1 187 // 0 1024

#define seePawn 104 // 0 300
#define seeKnight 450 // 0 800
#define seeBishop 414 // 0 800
#define seeRook 493 // 0 1200
#define seeQueen 982 // 0 2500

#define qsBadCapturesMargin 95 // 0 500
#define badCapturesBase 152 // 0 500
#define badCapturesHistory 150 // 0 500

#define moveOrderHistoryScore 349 // 0 1600
#define moveOrderPawn 84 // 0 300
#define moveOrderKnight 288 // 0 800
#define moveOrderBishop 305 // 0 800
#define moveOrderRook 431 // 0 1200
#define moveOrderQueen 917 // 0 2500


