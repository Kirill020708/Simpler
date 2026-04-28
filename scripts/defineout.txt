#pragma once


#define qsFPmargin 97 // 0 300
#define qsFPseeMargin -14 // -100 100

#define corrplexityMargin 72 // 0 300

#define rfpBaseD2 33 // 0 70
#define rfpImprovingD2 14 // 0 40
#define rfpCorrplexityD2 19 // 0 40
#define rfpWorseningD2 0 // 0 40
#define rfpCutnodeD2 0 // 0 40
#define rfpBaseD1 1 // 0 70
#define rfpImprovingD1 2 // 0 40
#define rfpCorrplexityD1 3 // 0 40
#define rfpWorseningD1 2 // 0 40
#define rfpCutnodeD1 2 // 0 40
#define rfpBaseD0 6 // 0 70
#define rfpImprovingD0 2 // 0 40
#define rfpCorrplexityD0 5 // 0 40
#define rfpWorseningD0 1 // 0 40
#define rfpCutnodeD0 2 // 0 40
#define rfpFail 452 // 0 1024

#define nmpBaseMargin 14 // 0 80
#define nmpDepthMargin 917 // 0 10240
#define nmpDepth2Margin 106 // 0 10240
#define nmpRbase 4803 // 0 10240 341
#define nmpRdepth 139 // 0 1024
#define nmpRimproving 175 // 0 3072
#define nmpRcutnode 339 // -4096 4096
#define nmpRmargin 168 // 50 500
#define nmpRmarginClamp 4733 // 512 10240

#define razorBaseD2 125 // 100 400
#define razorImprovingD2 34 // 0 70
#define razorBaseD1 9 // 0 300
#define razorImprovingD1 3 // 0 70
#define razorBaseD0 170 // 0 400
#define razorImprovingD0 4 // 0 70
#define razorFPmargin 46 // 0 150
#define razorRFPmargin 162 // 0 500

#define probcutBase 248 // 0 500
#define probcutImproving 60 // 0 100
#define probcutFail 959 // 0 1024

#define singextMarginDepth 991 // 0 1024
#define dextMarginBase 14740 // 0 15360
#define dextMarginHistory 8 // 0 10
#define trextMarginBase 68 // 0 80

#define lmpBaseD2 1059 // 0 4096
#define lmpImprovingD2 225 // 0 1024
#define lmpTTcaptureD2 442 // 0 1024
#define lmpBaseD1 267 // 0 4096
#define lmpImprovingD1 106 // 0 1024
#define lmpTTcaptureD1 102 // 0 1024
#define lmpBaseD0 3601 // 0 5120
#define lmpImprovingD0 10 // 0 1024
#define lmpTTcaptureD0 19 // 0 1024
#define lmpHistoryThreshold 198 // -1280 1280

#define historyBaseD2 188 // 0 800
#define historyImprovingD2 8 // 0 200
#define historyTTcaptureD2 8 // 0 200
#define historyBaseD1 254 // 0 800
#define historyImprovingD1 54 // 0 200
#define historyTTcaptureD1 63 // 0 200
#define historyBaseD0 97 // 0 800
#define historyImprovingD0 4 // 0 200
#define historyTTcaptureD0 27 // 0 200

#define fpBaseD2 207 // 0 500
#define fpImprovingD2 19 // 0 200
#define fpTTcaptureD2 99 // 0 200
#define fpHistoryD2 75 // 0 200
#define fpBaseD1 50 // 0 500
#define fpImprovingD1 2 // 0 200
#define fpTTcaptureD1 12 // 0 200
#define fpHistoryD1 22 // 0 200
#define fpBaseD0 17 // 0 500
#define fpImprovingD0 6 // 0 200
#define fpTTcaptureD0 2 // 0 200
#define fpHistoryD0 15 // 0 200

#define seeBaseD0 19 // 0 500
#define seeHistoryD0 19 // 0 200
#define seeBaseD1 189 // 0 500
#define seeHistoryD1 60 // 0 200
#define seeBaseD2 112 // 0 500
#define seeHistoryD2 4 // 0 200

#define lmrDivisor 510 // 0 2048
#define lmrBase 543 // 0 4096
#define lmrPv 1314 // 0 4096
#define lmrHistory 860 // 0 4096
#define lmrImproving 517 // 0 4096
#define lmrTTcapture 1495 // 0 4096
#define lmrCutnode 866 // 0 4096
#define lmrTtpv 616 // 0 4096
#define lmrCapture 1246 // 0 4096
#define lmrSee 2006 // 0 4096
#define lmrKiller 382 // 0 4096

#define historyBonusD0 438 // -1600 1600
#define historyBonusD1 194 // 0 1600
#define historyBonusD2 123 // 0 1600
#define historyMaluseD0 720 // -1600 1600
#define historyMaluseD1 165 // 0 1600
#define historyMaluseD2 0 // 0 1600

#define aspirationWindowSize 21 // 15 200
#define aspirationWindowMult 1840 // 1800 10240

#define bmStab0 2560 // 300 5120 *
#define bmStab1 1229 // 300 5120 *
#define bmStab2 922 // 300 5120 *
#define bmStab3 819 // 300 5120 *
#define bmStab4 768 // 300 5120 *
#define nodesTM 1741 // 1200 4096 *

#define historyScoreFromTo 595 // 0 2048
#define historyScorePieceTo 881 // 0 2048
#define historyScorePly1 1100 // 0 2048
#define historyScorePly2 1298 // 0 2048
#define historyScorePly4 905 // 0 2048

#define corrhistPawn 162 // 0 1024
#define corrhistMinor 160 // 0 1024
#define corrhistColor 145 // 0 1024
#define corrhistFromTo 49 // 0 1024
#define corrhistPly1 308 // 0 1024

#define seePawn 103 // 0 300
#define seeKnight 277 // 0 800
#define seeBishop 372 // 0 800
#define seeRook 414 // 0 1200
#define seeQueen 871 // 0 2500

#define qsBadCapturesMargin 107 // 0 500
#define badCapturesBase 216 // 0 500
#define badCapturesHistory 130 // 0 500

#define moveOrderHistoryScore 171 // 0 1600
#define moveOrderPawn 82 // 0 300
#define moveOrderKnight 311 // 0 800
#define moveOrderBishop 83 // 0 800
#define moveOrderRook 533 // 0 1200
#define moveOrderQueen 739 // 0 2500


