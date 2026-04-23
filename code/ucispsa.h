#pragma once

void printSPSAparams() {
    cout <<
    "option name qsFPmargin type spin default 124 min 0 max 300\n"
    "option name qsFPseeMargin type spin default -16 min -100 max 100\n"
    "option name corrplexityMargin type spin default 107 min 0 max 300\n"
    "option name rfpBaseD2 type spin default 36 min 0 max 70\n"
    "option name rfpImprovingD2 type spin default 8 min 0 max 40\n"
    "option name rfpCorrplexityD2 type spin default 20 min 0 max 40\n"
    "option name rfpWorseningD2 type spin default 2 min 0 max 40\n"
    "option name rfpCutnodeD2 type spin default 0 min 0 max 40\n"
    "option name rfpBaseD1 type spin default 9 min 0 max 70\n"
    "option name rfpImprovingD1 type spin default 3 min 0 max 40\n"
    "option name rfpCorrplexityD1 type spin default 0 min 0 max 40\n"
    "option name rfpWorseningD1 type spin default 1 min 0 max 40\n"
    "option name rfpCutnodeD1 type spin default 3 min 0 max 40\n"
    "option name rfpBaseD0 type spin default 11 min 0 max 70\n"
    "option name rfpImprovingD0 type spin default 3 min 0 max 40\n"
    "option name rfpCorrplexityD0 type spin default 4 min 0 max 40\n"
    "option name rfpWorseningD0 type spin default 0 min 0 max 40\n"
    "option name rfpCutnodeD0 type spin default 4 min 0 max 40\n"
    "option name rfpFail type spin default 446 min 0 max 1024\n"
    "option name nmpBaseMargin type spin default 22 min 0 max 80\n"
    "option name nmpDepthMargin type spin default 1034 min 0 max 10240\n"
    "option name nmpDepth2Margin type spin default 107 min 0 max 10240\n"
    "option name nmpRbase type spin default 4974 min 0 max 10240\n"
    "option name nmpRdepth type spin default 131 min 0 max 1024\n"
    "option name nmpRimproving type spin default 123 min 0 max 3072\n"
    "option name nmpRcutnode type spin default -67 min -4096 max 4096\n"
    "option name nmpRmargin type spin default 176 min 50 max 500\n"
    "option name nmpRmarginClamp type spin default 5778 min 512 max 10240\n"
    "option name razorBaseD2 type spin default 184 min 100 max 400\n"
    "option name razorImprovingD2 type spin default 27 min 0 max 70\n"
    "option name razorBaseD1 type spin default 24 min 0 max 300\n"
    "option name razorImprovingD1 type spin default 0 min 0 max 70\n"
    "option name razorBaseD0 type spin default 216 min 0 max 400\n"
    "option name razorImprovingD0 type spin default 6 min 0 max 70\n"
    "option name razorFPmargin type spin default 23 min 0 max 150\n"
    "option name razorRFPmargin type spin default 117 min 0 max 500\n"
    "option name probcutBase type spin default 180 min 0 max 500\n"
    "option name probcutImproving type spin default 44 min 0 max 100\n"
    "option name probcutFail type spin default 1005 min 0 max 1024\n"
    "option name singextMarginDepth type spin default 898 min 0 max 1024\n"
    "option name dextMarginBase type spin default 14537 min 0 max 15360\n"
    "option name dextMarginHistory type spin default 10 min 0 max 10\n"
    "option name trextMarginBase type spin default 69 min 0 max 80\n"
    "option name lmpBaseD2 type spin default 1626 min 0 max 4096\n"
    "option name lmpImprovingD2 type spin default 206 min 0 max 1024\n"
    "option name lmpTTcaptureD2 type spin default 655 min 0 max 1024\n"
    "option name lmpBaseD1 type spin default 565 min 0 max 4096\n"
    "option name lmpImprovingD1 type spin default 203 min 0 max 1024\n"
    "option name lmpTTcaptureD1 type spin default 22 min 0 max 1024\n"
    "option name lmpBaseD0 type spin default 3106 min 0 max 5120\n"
    "option name lmpImprovingD0 type spin default 135 min 0 max 1024\n"
    "option name lmpTTcaptureD0 type spin default 80 min 0 max 1024\n"
    "option name lmpHistoryThreshold type spin default -129 min -1280 max 1280\n"
    "option name historyBaseD2 type spin default 163 min 0 max 800\n"
    "option name historyImprovingD2 type spin default 33 min 0 max 200\n"
    "option name historyTTcaptureD2 type spin default 13 min 0 max 200\n"
    "option name historyBaseD1 type spin default 211 min 0 max 800\n"
    "option name historyImprovingD1 type spin default 39 min 0 max 200\n"
    "option name historyTTcaptureD1 type spin default 85 min 0 max 200\n"
    "option name historyBaseD0 type spin default 92 min 0 max 800\n"
    "option name historyImprovingD0 type spin default 1 min 0 max 200\n"
    "option name historyTTcaptureD0 type spin default 13 min 0 max 200\n"
    "option name fpBaseD2 type spin default 83 min 0 max 500\n"
    "option name fpImprovingD2 type spin default 17 min 0 max 200\n"
    "option name fpTTcaptureD2 type spin default 90 min 0 max 200\n"
    "option name fpHistoryD2 type spin default 81 min 0 max 200\n"
    "option name fpBaseD1 type spin default 23 min 0 max 500\n"
    "option name fpImprovingD1 type spin default 4 min 0 max 200\n"
    "option name fpTTcaptureD1 type spin default 18 min 0 max 200\n"
    "option name fpHistoryD1 type spin default 15 min 0 max 200\n"
    "option name fpBaseD0 type spin default 2 min 0 max 500\n"
    "option name fpImprovingD0 type spin default 3 min 0 max 200\n"
    "option name fpTTcaptureD0 type spin default 6 min 0 max 200\n"
    "option name fpHistoryD0 type spin default 26 min 0 max 200\n"
    "option name seeBaseD0 type spin default 2 min 0 max 500\n"
    "option name seeHistoryD0 type spin default 9 min 0 max 200\n"
    "option name seeBaseD1 type spin default 116 min 0 max 500\n"
    "option name seeHistoryD1 type spin default 58 min 0 max 200\n"
    "option name seeBaseD2 type spin default 70 min 0 max 500\n"
    "option name seeHistoryD2 type spin default 4 min 0 max 200\n"
    "option name lmrDivisor type spin default 219 min 0 max 2048\n"
    "option name lmrBase type spin default 644 min 0 max 4096\n"
    "option name lmrPv type spin default 384 min 0 max 4096\n"
    "option name lmrHistory type spin default 1287 min 0 max 4096\n"
    "option name lmrImproving type spin default 639 min 0 max 4096\n"
    "option name lmrTTcapture type spin default 939 min 0 max 4096\n"
    "option name lmrCutnode type spin default 960 min 0 max 4096\n"
    "option name lmrTtpv type spin default 743 min 0 max 4096\n"
    "option name lmrCapture type spin default 684 min 0 max 4096\n"
    "option name lmrSee type spin default 2088 min 0 max 4096\n"
    "option name lmrKiller type spin default 826 min 0 max 4096\n"
    "option name historyBonusD0 type spin default 475 min -1600 max 1600\n"
    "option name historyBonusD1 type spin default 137 min 0 max 1600\n"
    "option name historyBonusD2 type spin default 311 min 0 max 1600\n"
    "option name historyMaluseD0 type spin default 653 min -1600 max 1600\n"
    "option name historyMaluseD1 type spin default 164 min 0 max 1600\n"
    "option name historyMaluseD2 type spin default 7 min 0 max 1600\n"
    "option name aspirationWindowSize type spin default 54 min 15 max 200\n"
    "option name aspirationWindowMult type spin default 3132 min 1800 max 10240\n"
    "option name bmStab0 type spin default 2560 min 300 max 5120\n"
    "option name bmStab1 type spin default 1229 min 300 max 5120\n"
    "option name bmStab2 type spin default 922 min 300 max 5120\n"
    "option name bmStab3 type spin default 819 min 300 max 5120\n"
    "option name bmStab4 type spin default 768 min 300 max 5120\n"
    "option name nodesTM type spin default 1741 min 1200 max 4096\n"
    "option name historyScoreFromTo type spin default 926 min 0 max 2048\n"
    "option name historyScorePieceTo type spin default 782 min 0 max 2048\n"
    "option name historyScorePly1 type spin default 1084 min 0 max 2048\n"
    "option name historyScorePly2 type spin default 795 min 0 max 2048\n"
    "option name historyScorePly4 type spin default 777 min 0 max 2048\n"
    "option name corrhistPawn type spin default 84 min 0 max 1024\n"
    "option name corrhistMinor type spin default 12 min 0 max 1024\n"
    "option name corrhistColor type spin default 117 min 0 max 1024\n"
    "option name corrhistFromTo type spin default 20 min 0 max 1024\n"
    "option name corrhistPly1 type spin default 181 min 0 max 1024\n"
    "option name seePawn type spin default 118 min 0 max 300\n"
    "option name seeKnight type spin default 439 min 0 max 800\n"
    "option name seeBishop type spin default 410 min 0 max 800\n"
    "option name seeRook type spin default 506 min 0 max 1200\n"
    "option name seeQueen type spin default 878 min 0 max 2500\n"
    "option name qsBadCapturesMargin type spin default 100 min 0 max 500\n"
    "option name badCapturesBase type spin default 162 min 0 max 500\n"
    "option name badCapturesHistory type spin default 118 min 0 max 500\n"
    "option name moveOrderHistoryScore type spin default 387 min 0 max 1600\n"
    "option name moveOrderPawn type spin default 85 min 0 max 300\n"
    "option name moveOrderKnight type spin default 288 min 0 max 800\n"
    "option name moveOrderBishop type spin default 279 min 0 max 800\n"
    "option name moveOrderRook type spin default 443 min 0 max 1200\n"
    "option name moveOrderQueen type spin default 1002 min 0 max 2500\n"
;
}

void setParam(string name, int value) {
    if (name == "qsFPmargin") qsFPmargin = value;
    if (name == "qsFPseeMargin") qsFPseeMargin = value;
    if (name == "corrplexityMargin") corrplexityMargin = value;
    if (name == "rfpBaseD2") rfpBaseD2 = value;
    if (name == "rfpImprovingD2") rfpImprovingD2 = value;
    if (name == "rfpCorrplexityD2") rfpCorrplexityD2 = value;
    if (name == "rfpWorseningD2") rfpWorseningD2 = value;
    if (name == "rfpCutnodeD2") rfpCutnodeD2 = value;
    if (name == "rfpBaseD1") rfpBaseD1 = value;
    if (name == "rfpImprovingD1") rfpImprovingD1 = value;
    if (name == "rfpCorrplexityD1") rfpCorrplexityD1 = value;
    if (name == "rfpWorseningD1") rfpWorseningD1 = value;
    if (name == "rfpCutnodeD1") rfpCutnodeD1 = value;
    if (name == "rfpBaseD0") rfpBaseD0 = value;
    if (name == "rfpImprovingD0") rfpImprovingD0 = value;
    if (name == "rfpCorrplexityD0") rfpCorrplexityD0 = value;
    if (name == "rfpWorseningD0") rfpWorseningD0 = value;
    if (name == "rfpCutnodeD0") rfpCutnodeD0 = value;
    if (name == "rfpFail") rfpFail = value;
    if (name == "nmpBaseMargin") nmpBaseMargin = value;
    if (name == "nmpDepthMargin") nmpDepthMargin = value;
    if (name == "nmpDepth2Margin") nmpDepth2Margin = value;
    if (name == "nmpRbase") nmpRbase = value;
    if (name == "nmpRdepth") nmpRdepth = value;
    if (name == "nmpRimproving") nmpRimproving = value;
    if (name == "nmpRcutnode") nmpRcutnode = value;
    if (name == "nmpRmargin") nmpRmargin = value;
    if (name == "nmpRmarginClamp") nmpRmarginClamp = value;
    if (name == "razorBaseD2") razorBaseD2 = value;
    if (name == "razorImprovingD2") razorImprovingD2 = value;
    if (name == "razorBaseD1") razorBaseD1 = value;
    if (name == "razorImprovingD1") razorImprovingD1 = value;
    if (name == "razorBaseD0") razorBaseD0 = value;
    if (name == "razorImprovingD0") razorImprovingD0 = value;
    if (name == "razorFPmargin") razorFPmargin = value;
    if (name == "razorRFPmargin") razorRFPmargin = value;
    if (name == "probcutBase") probcutBase = value;
    if (name == "probcutImproving") probcutImproving = value;
    if (name == "probcutFail") probcutFail = value;
    if (name == "singextMarginDepth") singextMarginDepth = value;
    if (name == "dextMarginBase") dextMarginBase = value;
    if (name == "dextMarginHistory") dextMarginHistory = value;
    if (name == "trextMarginBase") trextMarginBase = value;
    if (name == "lmpBaseD2") lmpBaseD2 = value;
    if (name == "lmpImprovingD2") lmpImprovingD2 = value;
    if (name == "lmpTTcaptureD2") lmpTTcaptureD2 = value;
    if (name == "lmpBaseD1") lmpBaseD1 = value;
    if (name == "lmpImprovingD1") lmpImprovingD1 = value;
    if (name == "lmpTTcaptureD1") lmpTTcaptureD1 = value;
    if (name == "lmpBaseD0") lmpBaseD0 = value;
    if (name == "lmpImprovingD0") lmpImprovingD0 = value;
    if (name == "lmpTTcaptureD0") lmpTTcaptureD0 = value;
    if (name == "lmpHistoryThreshold") lmpHistoryThreshold = value;
    if (name == "historyBaseD2") historyBaseD2 = value;
    if (name == "historyImprovingD2") historyImprovingD2 = value;
    if (name == "historyTTcaptureD2") historyTTcaptureD2 = value;
    if (name == "historyBaseD1") historyBaseD1 = value;
    if (name == "historyImprovingD1") historyImprovingD1 = value;
    if (name == "historyTTcaptureD1") historyTTcaptureD1 = value;
    if (name == "historyBaseD0") historyBaseD0 = value;
    if (name == "historyImprovingD0") historyImprovingD0 = value;
    if (name == "historyTTcaptureD0") historyTTcaptureD0 = value;
    if (name == "fpBaseD2") fpBaseD2 = value;
    if (name == "fpImprovingD2") fpImprovingD2 = value;
    if (name == "fpTTcaptureD2") fpTTcaptureD2 = value;
    if (name == "fpHistoryD2") fpHistoryD2 = value;
    if (name == "fpBaseD1") fpBaseD1 = value;
    if (name == "fpImprovingD1") fpImprovingD1 = value;
    if (name == "fpTTcaptureD1") fpTTcaptureD1 = value;
    if (name == "fpHistoryD1") fpHistoryD1 = value;
    if (name == "fpBaseD0") fpBaseD0 = value;
    if (name == "fpImprovingD0") fpImprovingD0 = value;
    if (name == "fpTTcaptureD0") fpTTcaptureD0 = value;
    if (name == "fpHistoryD0") fpHistoryD0 = value;
    if (name == "seeBaseD0") seeBaseD0 = value;
    if (name == "seeHistoryD0") seeHistoryD0 = value;
    if (name == "seeBaseD1") seeBaseD1 = value;
    if (name == "seeHistoryD1") seeHistoryD1 = value;
    if (name == "seeBaseD2") seeBaseD2 = value;
    if (name == "seeHistoryD2") seeHistoryD2 = value;
    if (name == "lmrDivisor") lmrDivisor = value;
    if (name == "lmrBase") lmrBase = value;
    if (name == "lmrPv") lmrPv = value;
    if (name == "lmrHistory") lmrHistory = value;
    if (name == "lmrImproving") lmrImproving = value;
    if (name == "lmrTTcapture") lmrTTcapture = value;
    if (name == "lmrCutnode") lmrCutnode = value;
    if (name == "lmrTtpv") lmrTtpv = value;
    if (name == "lmrCapture") lmrCapture = value;
    if (name == "lmrSee") lmrSee = value;
    if (name == "lmrKiller") lmrKiller = value;
    if (name == "historyBonusD0") historyBonusD0 = value;
    if (name == "historyBonusD1") historyBonusD1 = value;
    if (name == "historyBonusD2") historyBonusD2 = value;
    if (name == "historyMaluseD0") historyMaluseD0 = value;
    if (name == "historyMaluseD1") historyMaluseD1 = value;
    if (name == "historyMaluseD2") historyMaluseD2 = value;
    if (name == "aspirationWindowSize") aspirationWindowSize = value;
    if (name == "aspirationWindowMult") aspirationWindowMult = value;
    if (name == "bmStab0") bmStab0 = value;
    if (name == "bmStab1") bmStab1 = value;
    if (name == "bmStab2") bmStab2 = value;
    if (name == "bmStab3") bmStab3 = value;
    if (name == "bmStab4") bmStab4 = value;
    if (name == "nodesTM") nodesTM = value;
    if (name == "historyScoreFromTo") historyScoreFromTo = value;
    if (name == "historyScorePieceTo") historyScorePieceTo = value;
    if (name == "historyScorePly1") historyScorePly1 = value;
    if (name == "historyScorePly2") historyScorePly2 = value;
    if (name == "historyScorePly4") historyScorePly4 = value;
    if (name == "corrhistPawn") corrhistPawn = value;
    if (name == "corrhistMinor") corrhistMinor = value;
    if (name == "corrhistColor") corrhistColor = value;
    if (name == "corrhistFromTo") corrhistFromTo = value;
    if (name == "corrhistPly1") corrhistPly1 = value;
    if (name == "seePawn") seePawn = value;
    if (name == "seeKnight") seeKnight = value;
    if (name == "seeBishop") seeBishop = value;
    if (name == "seeRook") seeRook = value;
    if (name == "seeQueen") seeQueen = value;
    if (name == "qsBadCapturesMargin") qsBadCapturesMargin = value;
    if (name == "badCapturesBase") badCapturesBase = value;
    if (name == "badCapturesHistory") badCapturesHistory = value;
    if (name == "moveOrderHistoryScore") moveOrderHistoryScore = value;
    if (name == "moveOrderPawn") moveOrderPawn = value;
    if (name == "moveOrderKnight") moveOrderKnight = value;
    if (name == "moveOrderBishop") moveOrderBishop = value;
    if (name == "moveOrderRook") moveOrderRook = value;
    if (name == "moveOrderQueen") moveOrderQueen = value;
}
