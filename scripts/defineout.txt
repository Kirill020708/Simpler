#pragma once


#define qsFPmargin 103 // 0 300
#define qsFPseeMargin 1 // -100 100

#define corrplexityMargin 101 // 0 300

#define rfpBaseD2 29 // 0 70
#define rfpImprovingD2 15 // 0 40
#define rfpCorrplexityD2 20 // 0 40
#define rfpWorseningD2 0 // 0 40
#define rfpCutnodeD2 0 // 0 40
#define rfpBaseD1 1 // 0 70
#define rfpImprovingD1 0 // 0 40
#define rfpCorrplexityD1 1 // 0 40
#define rfpWorseningD1 1 // 0 40
#define rfpCutnodeD1 0 // 0 40
#define rfpBaseD0 1 // 0 70
#define rfpImprovingD0 0 // 0 40
#define rfpCorrplexityD0 0 // 0 40
#define rfpWorseningD0 0 // 0 40
#define rfpCutnodeD0 1 // 0 40
#define rfpFail 507 // 0 1024

#define nmpBaseMargin 19 // 0 80
#define nmpDepthMargin 1039 // 0 10240
#define nmpDepth2Margin 23 // 0 10240
#define nmpRbase 5208 // 0 10240 341
#define nmpRdepth 197 // 0 1024
#define nmpRimproving 70 // 0 3072
#define nmpRcutnode -38 // -4096 4096
#define nmpRmargin 206 // 50 500
#define nmpRmarginClamp 5130 // 512 10240

#define razorBaseD2 147 // 100 400
#define razorImprovingD2 30 // 0 70
#define razorBaseD1 0 // 0 300
#define razorImprovingD1 1 // 0 70
#define razorBaseD0 198 // 0 400
#define razorImprovingD0 0 // 0 70
#define razorFPmargin 53 // 0 150
#define razorRFPmargin 205 // 0 500

#define probcutBase 192 // 0 500
#define probcutImproving 50 // 0 100
#define probcutFail 1012 // 0 1024

#define singextMarginDepth 1012 // 0 1024
#define dextMarginBase 15292 // 0 15360
#define dextMarginHistory 10 // 0 10
#define trextMarginBase 80 // 0 80

#define lmpBaseD2 1007 // 0 4096
#define lmpImprovingD2 251 // 0 1024
#define lmpTTcaptureD2 518 // 0 1024
#define lmpBaseD1 37 // 0 4096
#define lmpImprovingD1 0 // 0 1024
#define lmpTTcaptureD1 4 // 0 1024
#define lmpBaseD0 3055 // 0 5120
#define lmpImprovingD0 18 // 0 1024
#define lmpTTcaptureD0 8 // 0 1024
#define lmpHistoryThreshold 1 // -1280 1280

#define historyBaseD2 16 // 0 800
#define historyImprovingD2 1 // 0 200
#define historyTTcaptureD2 0 // 0 200
#define historyBaseD1 203 // 0 800
#define historyImprovingD1 51 // 0 200
#define historyTTcaptureD1 62 // 0 200
#define historyBaseD0 6 // 0 800
#define historyImprovingD0 4 // 0 200
#define historyTTcaptureD0 3 // 0 200

#define fpBaseD2 145 // 0 500
#define fpImprovingD2 0 // 0 200
#define fpTTcaptureD2 100 // 0 200
#define fpHistoryD2 70 // 0 200
#define fpBaseD1 8 // 0 500
#define fpImprovingD1 3 // 0 200
#define fpTTcaptureD1 0 // 0 200
#define fpHistoryD1 1 // 0 200
#define fpBaseD0 6 // 0 500
#define fpImprovingD0 1 // 0 200
#define fpTTcaptureD0 3 // 0 200
#define fpHistoryD0 2 // 0 200

#define seeBaseD0 6 // 0 500
#define seeHistoryD0 1 // 0 200
#define seeBaseD1 105 // 0 500
#define seeHistoryD1 74 // 0 200
#define seeBaseD2 15 // 0 500
#define seeHistoryD2 0 // 0 200

#define lmrDivisor 356 // 0 2048
#define lmrBase 503 // 0 4096
#define lmrPv 1001 // 0 4096
#define lmrHistory 1498 // 0 4096
#define lmrImproving 505 // 0 4096
#define lmrTTcapture 921 // 0 4096
#define lmrCutnode 994 // 0 4096
#define lmrTtpv 990 // 0 4096
#define lmrCapture 1058 // 0 4096
#define lmrSee 2060 // 0 4096
#define lmrKiller 1097 // 0 4096

#define historyBonusD0 25 // -1600 1600
#define historyBonusD1 162 // 0 1600
#define historyBonusD2 42 // 0 1600
#define historyMaluseD0 50 // -1600 1600
#define historyMaluseD1 160 // 0 1600
#define historyMaluseD2 3 // 0 1600

#define aspirationWindowSize 25 // 15 200
#define aspirationWindowMult 2016 // 1800 10240

#define bmStab0 2560 // 300 5120 *
#define bmStab1 1229 // 300 5120 *
#define bmStab2 922 // 300 5120 *
#define bmStab3 819 // 300 5120 *
#define bmStab4 768 // 300 5120 *
#define nodesTM 1741 // 1200 4096 *

#define historyScoreFromTo 1001 // 0 2048
#define historyScorePieceTo 1017 // 0 2048
#define historyScorePly1 1022 // 0 2048
#define historyScorePly2 985 // 0 2048
#define historyScorePly4 1071 // 0 2048

#define corrhistPawn 171 // 0 1024
#define corrhistMinor 174 // 0 1024
#define corrhistColor 165 // 0 1024
#define corrhistFromTo 175 // 0 1024
#define corrhistPly1 163 // 0 1024

#define seePawn 99 // 0 300
#define seeKnight 291 // 0 800
#define seeBishop 294 // 0 800
#define seeRook 471 // 0 1200
#define seeQueen 1015 // 0 2500

#define qsBadCapturesMargin 103 // 0 500
#define badCapturesBase 211 // 0 500
#define badCapturesHistory 101 // 0 500

#define moveOrderHistoryScore 329 // 0 1600
#define moveOrderPawn 99 // 0 300
#define moveOrderKnight 301 // 0 800
#define moveOrderBishop 310 // 0 800
#define moveOrderRook 513 // 0 1200
#define moveOrderQueen 912 // 0 2500


