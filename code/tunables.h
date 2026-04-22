#pragma once


#define qsFPmargin 124 // 0 300
#define qsFPseeMargin -16 // -100 100

#define corrplexityMargin 107 // 0 300

#define rfpBaseD2 36 // 0 70
#define rfpImprovingD2 8 // 0 40
#define rfpCorrplexityD2 20 // 0 40
#define rfpWorseningD2 2 // 0 40
#define rfpCutnodeD2 0 // 0 40
#define rfpBaseD1 9 // 0 70
#define rfpImprovingD1 3 // 0 40
#define rfpCorrplexityD1 0 // 0 40
#define rfpWorseningD1 1 // 0 40
#define rfpCutnodeD1 3 // 0 40
#define rfpBaseD0 11 // 0 70
#define rfpImprovingD0 3 // 0 40
#define rfpCorrplexityD0 4 // 0 40
#define rfpWorseningD0 0 // 0 40
#define rfpCutnodeD0 4 // 0 40
#define rfpFail 446 // 0 1024

#define nmpBaseMargin 22 // 0 80
#define nmpDepthMargin 1034 // 0 10240
#define nmpDepth2Margin 107 // 0 10240
#define nmpRbase 4974 // 0 10240 341
#define nmpRdepth 131 // 0 1024
#define nmpRimproving 123 // 0 3072
#define nmpRcutnode -67 // -4096 4096
#define nmpRmargin 176 // 50 500
#define nmpRmarginClamp 5778 // 512 10240

#define razorBaseD2 184 // 100 400
#define razorImprovingD2 27 // 0 70
#define razorBaseD1 24 // 0 300
#define razorImprovingD1 0 // 0 70
#define razorBaseD0 216 // 0 400
#define razorImprovingD0 6 // 0 70
#define razorFPmargin 23 // 0 150
#define razorRFPmargin 117 // 0 500

#define probcutBase 180 // 0 500
#define probcutImproving 44 // 0 100
#define probcutFail 1005 // 0 1024

#define singextMarginDepth 898 // 0 1024
#define dextMarginBase 14537 // 0 15360
#define dextMarginHistory 10 // 0 10
#define trextMarginBase 69 // 0 80

#define lmpBaseD2 1626 // 0 4096
#define lmpImprovingD2 206 // 0 1024
#define lmpTTcaptureD2 655 // 0 1024
#define lmpBaseD1 565 // 0 4096
#define lmpImprovingD1 203 // 0 1024
#define lmpTTcaptureD1 22 // 0 1024
#define lmpBaseD0 3106 // 0 5120
#define lmpImprovingD0 135 // 0 1024
#define lmpTTcaptureD0 80 // 0 1024
#define lmpHistoryThreshold -129 // -1280 1280

#define historyBaseD2 163 // 0 800
#define historyImprovingD2 33 // 0 200
#define historyTTcaptureD2 13 // 0 200
#define historyBaseD1 211 // 0 800
#define historyImprovingD1 39 // 0 200
#define historyTTcaptureD1 85 // 0 200
#define historyBaseD0 92 // 0 800
#define historyImprovingD0 1 // 0 200
#define historyTTcaptureD0 13 // 0 200

#define fpBaseD2 83 // 0 500
#define fpImprovingD2 17 // 0 200
#define fpTTcaptureD2 90 // 0 200
#define fpHistoryD2 81 // 0 200
#define fpBaseD1 23 // 0 500
#define fpImprovingD1 4 // 0 200
#define fpTTcaptureD1 18 // 0 200
#define fpHistoryD1 15 // 0 200
#define fpBaseD0 2 // 0 500
#define fpImprovingD0 3 // 0 200
#define fpTTcaptureD0 6 // 0 200
#define fpHistoryD0 26 // 0 200

#define seeBaseD0 2 // 0 500
#define seeHistoryD0 9 // 0 200
#define seeBaseD1 116 // 0 500
#define seeHistoryD1 58 // 0 200
#define seeBaseD2 70 // 0 500
#define seeHistoryD2 4 // 0 200

#define lmrDivisor 219 // 0 2048
#define lmrBase 644 // 0 4096
#define lmrPv 384 // 0 4096
#define lmrHistory 1287 // 0 4096
#define lmrImproving 639 // 0 4096
#define lmrTTcapture 939 // 0 4096
#define lmrCutnode 960 // 0 4096
#define lmrTtpv 743 // 0 4096
#define lmrCapture 684 // 0 4096
#define lmrSee 2088 // 0 4096
#define lmrKiller 826 // 0 4096

#define historyBonusD0 475 // -1600 1600
#define historyBonusD1 137 // 0 1600
#define historyBonusD2 311 // 0 1600
#define historyMaluseD0 653 // -1600 1600
#define historyMaluseD1 164 // 0 1600
#define historyMaluseD2 7 // 0 1600

#define aspirationWindowSize 54 // 15 200
#define aspirationWindowMult 3132 // 1800 10240

#define bmStab0 2560 // 300 5120 *
#define bmStab1 1229 // 300 5120 *
#define bmStab2 922 // 300 5120 *
#define bmStab3 819 // 300 5120 *
#define bmStab4 768 // 300 5120 *
#define nodesTM 1741 // 1200 4096 *

#define historyScoreFromTo 926 // 0 2048
#define historyScorePieceTo 782 // 0 2048
#define historyScorePly1 1084 // 0 2048
#define historyScorePly2 795 // 0 2048
#define historyScorePly4 777 // 0 2048

#define corrhistPawn 84 // 0 1024
#define corrhistMinor 12 // 0 1024
#define corrhistColor 117 // 0 1024
#define corrhistFromTo 20 // 0 1024
#define corrhistPly1 181 // 0 1024

#define seePawn 118 // 0 300
#define seeKnight 439 // 0 800
#define seeBishop 410 // 0 800
#define seeRook 506 // 0 1200
#define seeQueen 878 // 0 2500

#define qsBadCapturesMargin 100 // 0 500
#define badCapturesBase 162 // 0 500
#define badCapturesHistory 118 // 0 500

#define moveOrderHistoryScore 387 // 0 1600
#define moveOrderPawn 85 // 0 300
#define moveOrderKnight 288 // 0 800
#define moveOrderBishop 279 // 0 800
#define moveOrderRook 443 // 0 1200
#define moveOrderQueen 1002 // 0 2500


