#pragma once


#define qsFPmargin 105 // 0 300
#define qsFPseeMargin 0 // -100 100

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
#define rfpBaseD0 1 // 0 70
#define rfpImprovingD0 0 // 0 40
#define rfpCorrplexityD0 0 // 0 40
#define rfpWorseningD0 0 // 0 40
#define rfpCutnodeD0 1 // 0 40
#define rfpFail 513 // 0 1024

#define nmpBaseMargin 19 // 0 80
#define nmpDepthMargin 1033 // 0 10240
#define nmpDepth2Margin 8 // 0 10240
#define nmpRbase 5182 // 0 10240 341
#define nmpRdepth 198 // 0 1024
#define nmpRimproving 90 // 0 3072
#define nmpRcutnode -59 // -4096 4096
#define nmpRmargin 204 // 50 500
#define nmpRmarginClamp 5139 // 512 10240

#define razorBaseD2 149 // 100 400
#define razorImprovingD2 30 // 0 70
#define razorBaseD1 0 // 0 300
#define razorImprovingD1 1 // 0 70
#define razorBaseD0 198 // 0 400
#define razorImprovingD0 0 // 0 70
#define razorFPmargin 53 // 0 150
#define razorRFPmargin 202 // 0 500

#define probcutBase 190 // 0 500
#define probcutImproving 50 // 0 100
#define probcutFail 1015 // 0 1024

#define singextMarginDepth 1012 // 0 1024
#define dextMarginBase 15249 // 0 15360
#define dextMarginHistory 10 // 0 10
#define trextMarginBase 80 // 0 80

#define lmpBaseD2 988 // 0 4096
#define lmpImprovingD2 249 // 0 1024
#define lmpTTcaptureD2 512 // 0 1024
#define lmpBaseD1 27 // 0 4096
#define lmpImprovingD1 0 // 0 1024
#define lmpTTcaptureD1 9 // 0 1024
#define lmpBaseD0 3059 // 0 5120
#define lmpImprovingD0 13 // 0 1024
#define lmpTTcaptureD0 7 // 0 1024
#define lmpHistoryThreshold 8 // -1280 1280

#define historyBaseD2 9 // 0 800
#define historyImprovingD2 1 // 0 200
#define historyTTcaptureD2 1 // 0 200
#define historyBaseD1 195 // 0 800
#define historyImprovingD1 53 // 0 200
#define historyTTcaptureD1 60 // 0 200
#define historyBaseD0 5 // 0 800
#define historyImprovingD0 4 // 0 200
#define historyTTcaptureD0 3 // 0 200

#define fpBaseD2 142 // 0 500
#define fpImprovingD2 0 // 0 200
#define fpTTcaptureD2 99 // 0 200
#define fpHistoryD2 71 // 0 200
#define fpBaseD1 6 // 0 500
#define fpImprovingD1 3 // 0 200
#define fpTTcaptureD1 0 // 0 200
#define fpHistoryD1 2 // 0 200
#define fpBaseD0 6 // 0 500
#define fpImprovingD0 0 // 0 200
#define fpTTcaptureD0 3 // 0 200
#define fpHistoryD0 1 // 0 200

#define seeBaseD0 8 // 0 500
#define seeHistoryD0 1 // 0 200
#define seeBaseD1 105 // 0 500
#define seeHistoryD1 73 // 0 200
#define seeBaseD2 15 // 0 500
#define seeHistoryD2 1 // 0 200

#define lmrDivisor 353 // 0 2048
#define lmrBase 501 // 0 4096
#define lmrPv 991 // 0 4096
#define lmrHistory 1492 // 0 4096
#define lmrImproving 506 // 0 4096
#define lmrTTcapture 921 // 0 4096
#define lmrCutnode 1004 // 0 4096
#define lmrTtpv 984 // 0 4096
#define lmrCapture 1037 // 0 4096
#define lmrSee 2055 // 0 4096
#define lmrKiller 1106 // 0 4096

#define historyBonusD0 20 // -1600 1600
#define historyBonusD1 161 // 0 1600
#define historyBonusD2 44 // 0 1600
#define historyMaluseD0 25 // -1600 1600
#define historyMaluseD1 159 // 0 1600
#define historyMaluseD2 9 // 0 1600

#define aspirationWindowSize 24 // 15 200
#define aspirationWindowMult 2027 // 1800 10240

#define bmStab0 2560 // 300 5120 *
#define bmStab1 1229 // 300 5120 *
#define bmStab2 922 // 300 5120 *
#define bmStab3 819 // 300 5120 *
#define bmStab4 768 // 300 5120 *
#define nodesTM 1741 // 1200 4096 *

#define historyScoreFromTo 1006 // 0 2048
#define historyScorePieceTo 1015 // 0 2048
#define historyScorePly1 1024 // 0 2048
#define historyScorePly2 998 // 0 2048
#define historyScorePly4 1071 // 0 2048

#define corrhistPawn 171 // 0 1024
#define corrhistMinor 174 // 0 1024
#define corrhistColor 166 // 0 1024
#define corrhistFromTo 176 // 0 1024
#define corrhistPly1 164 // 0 1024

#define seePawn 100 // 0 300
#define seeKnight 290 // 0 800
#define seeBishop 296 // 0 800
#define seeRook 471 // 0 1200
#define seeQueen 1026 // 0 2500

#define qsBadCapturesMargin 102 // 0 500
#define badCapturesBase 212 // 0 500
#define badCapturesHistory 101 // 0 500

#define moveOrderHistoryScore 331 // 0 1600
#define moveOrderPawn 99 // 0 300
#define moveOrderKnight 305 // 0 800
#define moveOrderBishop 309 // 0 800
#define moveOrderRook 507 // 0 1200
#define moveOrderQueen 909 // 0 2500


