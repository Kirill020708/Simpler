#pragma once


#define qsFPmargin 104 // 0 300
#define qsFPseeMargin -1 // -100 100

#define corrplexityMargin 103 // 0 300

#define rfpBaseD2 29 // 0 70
#define rfpImprovingD2 15 // 0 40
#define rfpCorrplexityD2 20 // 0 40
#define rfpWorseningD2 0 // 0 40
#define rfpCutnodeD2 0 // 0 40
#define rfpBaseD1 0 // 0 70
#define rfpImprovingD1 0 // 0 40
#define rfpCorrplexityD1 0 // 0 40
#define rfpWorseningD1 1 // 0 40
#define rfpCutnodeD1 0 // 0 40
#define rfpBaseD0 2 // 0 70
#define rfpImprovingD0 0 // 0 40
#define rfpCorrplexityD0 0 // 0 40
#define rfpWorseningD0 0 // 0 40
#define rfpCutnodeD0 1 // 0 40
#define rfpFail 520 // 0 1024

#define nmpBaseMargin 19 // 0 80
#define nmpDepthMargin 1027 // 0 10240
#define nmpDepth2Margin 0 // 0 10240
#define nmpRbase 5099 // 0 10240 341
#define nmpRdepth 197 // 0 1024
#define nmpRimproving 75 // 0 3072
#define nmpRcutnode -73 // -4096 4096
#define nmpRmargin 201 // 50 500
#define nmpRmarginClamp 5154 // 512 10240

#define razorBaseD2 147 // 100 400
#define razorImprovingD2 30 // 0 70
#define razorBaseD1 1 // 0 300
#define razorImprovingD1 1 // 0 70
#define razorBaseD0 197 // 0 400
#define razorImprovingD0 0 // 0 70
#define razorFPmargin 52 // 0 150
#define razorRFPmargin 197 // 0 500

#define probcutBase 190 // 0 500
#define probcutImproving 50 // 0 100
#define probcutFail 1017 // 0 1024

#define singextMarginDepth 1008 // 0 1024
#define dextMarginBase 15144 // 0 15360
#define dextMarginHistory 10 // 0 10
#define trextMarginBase 79 // 0 80

#define lmpBaseD2 986 // 0 4096
#define lmpImprovingD2 239 // 0 1024
#define lmpTTcaptureD2 516 // 0 1024
#define lmpBaseD1 21 // 0 4096
#define lmpImprovingD1 6 // 0 1024
#define lmpTTcaptureD1 7 // 0 1024
#define lmpBaseD0 3064 // 0 5120
#define lmpImprovingD0 8 // 0 1024
#define lmpTTcaptureD0 7 // 0 1024
#define lmpHistoryThreshold 5 // -1280 1280

#define historyBaseD2 1 // 0 800
#define historyImprovingD2 1 // 0 200
#define historyTTcaptureD2 1 // 0 200
#define historyBaseD1 197 // 0 800
#define historyImprovingD1 55 // 0 200
#define historyTTcaptureD1 61 // 0 200
#define historyBaseD0 3 // 0 800
#define historyImprovingD0 3 // 0 200
#define historyTTcaptureD0 4 // 0 200

#define fpBaseD2 142 // 0 500
#define fpImprovingD2 1 // 0 200
#define fpTTcaptureD2 98 // 0 200
#define fpHistoryD2 71 // 0 200
#define fpBaseD1 4 // 0 500
#define fpImprovingD1 2 // 0 200
#define fpTTcaptureD1 0 // 0 200
#define fpHistoryD1 1 // 0 200
#define fpBaseD0 5 // 0 500
#define fpImprovingD0 0 // 0 200
#define fpTTcaptureD0 3 // 0 200
#define fpHistoryD0 1 // 0 200

#define seeBaseD0 7 // 0 500
#define seeHistoryD0 0 // 0 200
#define seeBaseD1 103 // 0 500
#define seeHistoryD1 73 // 0 200
#define seeBaseD2 12 // 0 500
#define seeHistoryD2 0 // 0 200

#define lmrDivisor 350 // 0 2048
#define lmrBase 501 // 0 4096
#define lmrPv 968 // 0 4096
#define lmrHistory 1494 // 0 4096
#define lmrImproving 504 // 0 4096
#define lmrTTcapture 943 // 0 4096
#define lmrCutnode 1001 // 0 4096
#define lmrTtpv 1015 // 0 4096
#define lmrCapture 1036 // 0 4096
#define lmrSee 2057 // 0 4096
#define lmrKiller 1082 // 0 4096

#define historyBonusD0 17 // -1600 1600
#define historyBonusD1 161 // 0 1600
#define historyBonusD2 50 // 0 1600
#define historyMaluseD0 -20 // -1600 1600
#define historyMaluseD1 158 // 0 1600
#define historyMaluseD2 0 // 0 1600

#define aspirationWindowSize 24 // 15 200
#define aspirationWindowMult 2029 // 1800 10240

#define bmStab0 2560 // 300 5120 *
#define bmStab1 1229 // 300 5120 *
#define bmStab2 922 // 300 5120 *
#define bmStab3 819 // 300 5120 *
#define bmStab4 768 // 300 5120 *
#define nodesTM 1741 // 1200 4096 *

#define historyScoreFromTo 1012 // 0 2048
#define historyScorePieceTo 1015 // 0 2048
#define historyScorePly1 1023 // 0 2048
#define historyScorePly2 1002 // 0 2048
#define historyScorePly4 1074 // 0 2048

#define corrhistPawn 171 // 0 1024
#define corrhistMinor 171 // 0 1024
#define corrhistColor 169 // 0 1024
#define corrhistFromTo 175 // 0 1024
#define corrhistPly1 163 // 0 1024

#define seePawn 101 // 0 300
#define seeKnight 290 // 0 800
#define seeBishop 297 // 0 800
#define seeRook 482 // 0 1200
#define seeQueen 1025 // 0 2500

#define qsBadCapturesMargin 101 // 0 500
#define badCapturesBase 215 // 0 500
#define badCapturesHistory 102 // 0 500

#define moveOrderHistoryScore 330 // 0 1600
#define moveOrderPawn 98 // 0 300
#define moveOrderKnight 304 // 0 800
#define moveOrderBishop 305 // 0 800
#define moveOrderRook 493 // 0 1200
#define moveOrderQueen 920 // 0 2500


