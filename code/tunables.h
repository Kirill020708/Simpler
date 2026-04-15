#pragma once


#define qsFPmargin 100 // 0 300
#define qsFPseeMargin 0 // -100 100

#define corrplexityMargin 100 // 0 300

#define rfpBaseD2 30 // 0 70
#define rfpImprovingD2 15 // 0 40
#define rfpCorrplexityD2 20 // 0 40
#define rfpWorseningD2 0 // 0 40
#define rfpCutnodeD2 0 // 0 40
#define rfpBaseD1 0 // 0 70
#define rfpImprovingD1 0 // 0 40
#define rfpCorrplexityD1 0 // 0 40
#define rfpWorseningD1 0 // 0 40
#define rfpCutnodeD1 0 // 0 40
#define rfpBaseD0 0 // 0 70
#define rfpImprovingD0 0 // 0 40
#define rfpCorrplexityD0 0 // 0 40
#define rfpWorseningD0 0 // 0 40
#define rfpCutnodeD0 0 // 0 40
#define rfpFail 512 // 0 1024

#define nmpBaseMargin 20 // 0 80
#define nmpDepthMargin 1024 // 0 10240
#define nmpDepth2Margin 0 // 0 10240
#define nmpRbase 5120 // 0 10240 341
#define nmpRdepth 205 // 0 1024
#define nmpRimproving 0 // 0 3072
#define nmpRcutnode 0 // -4096 4096
#define nmpRmargin 200 // 50 500
#define nmpRmarginClamp 5120 // 512 10240

#define razorBaseD2 150 // 100 400
#define razorImprovingD2 30 // 0 70
#define razorBaseD1 0 // 0 300
#define razorImprovingD1 0 // 0 70
#define razorBaseD0 200 // 0 400
#define razorImprovingD0 0 // 0 70
#define razorFPmargin 50 // 0 150
#define razorRFPmargin 200 // 0 500

#define probcutBase 200 // 0 500
#define probcutImproving 50 // 0 100
#define probcutFail 1024 // 0 1024

#define singextMarginDepth 1024 // 0 1024
#define dextMarginBase 15360 // 0 15360
#define dextMarginHistory 10 // 0 10
#define trextMarginBase 80 // 0 80

#define lmpBaseD2 1024 // 0 4096
#define lmpImprovingD2 256 // 0 1024
#define lmpTTcaptureD2 512 // 0 1024
#define lmpBaseD1 0 // 0 4096
#define lmpImprovingD1 0 // 0 1024
#define lmpTTcaptureD1 0 // 0 1024
#define lmpBaseD0 3072 // 0 5120
#define lmpImprovingD0 0 // 0 1024
#define lmpTTcaptureD0 0 // 0 1024
#define lmpHistoryThreshold 0 // -1280 1280

#define historyBaseD2 0 // 0 800
#define historyImprovingD2 0 // 0 200
#define historyTTcaptureD2 0 // 0 200
#define historyBaseD1 200 // 0 800
#define historyImprovingD1 50 // 0 200
#define historyTTcaptureD1 60 // 0 200
#define historyBaseD0 0 // 0 800
#define historyImprovingD0 0 // 0 200
#define historyTTcaptureD0 0 // 0 200

#define fpBaseD2 150 // 0 500
#define fpImprovingD2 0 // 0 200
#define fpTTcaptureD2 100 // 0 200
#define fpHistoryD2 75 // 0 200
#define fpBaseD1 0 // 0 500
#define fpImprovingD1 0 // 0 200
#define fpTTcaptureD1 0 // 0 200
#define fpHistoryD1 0 // 0 200
#define fpBaseD0 0 // 0 500
#define fpImprovingD0 0 // 0 200
#define fpTTcaptureD0 0 // 0 200
#define fpHistoryD0 0 // 0 200

#define seeBaseD0 0 // 0 500
#define seeHistoryD0 0 // 0 200
#define seeBaseD1 100 // 0 500
#define seeHistoryD1 70 // 0 200
#define seeBaseD2 0 // 0 500
#define seeHistoryD2 0 // 0 200

#define lmrDivisor 341 // 0 2048
#define lmrBase 512 // 0 4096
#define lmrPv 1024 // 0 4096
#define lmrHistory 1536 // 0 4096
#define lmrImproving 512 // 0 4096
#define lmrTTcapture 1024 // 0 4096
#define lmrCutnode 1024 // 0 4096
#define lmrTtpv 1024 // 0 4096
#define lmrCapture 1024 // 0 4096
#define lmrSee 2097 // 0 4096
#define lmrKiller 1024 // 0 4096

#define historyBonusD0 0 // -1600 1600
#define historyBonusD1 160 // 0 1600
#define historyBonusD2 0 // 0 1600
#define historyMaluseD0 0 // -1600 1600
#define historyMaluseD1 160 // 0 1600
#define historyMaluseD2 0 // 0 1600

#define aspirationWindowSize 25 // 3 200
#define aspirationWindowMult 2048 // 1100 10240

#define bmStab0 2560 // 300 5120 *
#define bmStab1 1229 // 300 5120 *
#define bmStab2 922 // 300 5120 *
#define bmStab3 819 // 300 5120 *
#define bmStab4 768 // 300 5120 *
#define nodesTM 1741 // 1200 4096 *

#define historyScoreFromTo 1024 // 0 2048
#define historyScorePieceTo 1024 // 0 2048
#define historyScorePly1 1024 // 0 2048
#define historyScorePly2 1024 // 0 2048
#define historyScorePly4 1024 // 0 2048

#define corrhistPawn 171 // 0 1024
#define corrhistMinor 171 // 0 1024
#define corrhistColor 171 // 0 1024
#define corrhistFromTo 171 // 0 1024
#define corrhistPly1 171 // 0 1024

#define seePawn 100 // 0 300
#define seeKnight 300 // 0 800
#define seeBishop 300 // 0 800
#define seeRook 500 // 0 1200
#define seeQueen 1000 // 0 2500

#define qsBadCapturesMargin 100 // 0 500
#define badCapturesBase 220 // 0 500
#define badCapturesHistory 100 // 0 500

#define moveOrderHistoryScore 320 // 0 1600
#define moveOrderPawn 100 // 0 300
#define moveOrderKnight 300 // 0 800
#define moveOrderBishop 300 // 0 800
#define moveOrderRook 500 // 0 1200
#define moveOrderQueen 900 // 0 2500


