#include <Arduino.h>

#ifndef SINE_H
#define SINE_H

const  uint16_t sine_table[2048] {
    32768,
    32667,
    32566,
    32466,
    32365,
    32265,
    32164,
    32064,
    31963,
    31863,
    31762,
    31662,
    31561,
    31461,
    31361,
    31260,
    31160,
    31059,
    30959,
    30859,
    30758,
    30658,
    30558,
    30457,
    30357,
    30257,
    30157,
    30056,
    29956,
    29856,
    29756,
    29656,
    29556,
    29456,
    29356,
    29256,
    29156,
    29056,
    28956,
    28856,
    28756,
    28657,
    28557,
    28457,
    28358,
    28258,
    28158,
    28059,
    27960,
    27860,
    27761,
    27661,
    27562,
    27463,
    27364,
    27265,
    27166,
    27066,
    26968,
    26869,
    26770,
    26671,
    26572,
    26474,
    26375,
    26276,
    26178,
    26079,
    25981,
    25883,
    25784,
    25686,
    25588,
    25490,
    25392,
    25294,
    25196,
    25099,
    25001,
    24903,
    24806,
    24708,
    24611,
    24513,
    24416,
    24319,
    24222,
    24125,
    24028,
    23931,
    23834,
    23738,
    23641,
    23545,
    23448,
    23352,
    23256,
    23159,
    23063,
    22967,
    22872,
    22776,
    22680,
    22584,
    22489,
    22394,
    22298,
    22203,
    22108,
    22013,
    21918,
    21823,
    21728,
    21634,
    21539,
    21445,
    21351,
    21257,
    21162,
    21068,
    20975,
    20881,
    20787,
    20694,
    20600,
    20507,
    20414,
    20321,
    20228,
    20135,
    20042,
    19950,
    19857,
    19765,
    19673,
    19581,
    19489,
    19397,
    19305,
    19214,
    19122,
    19031,
    18940,
    18849,
    18758,
    18667,
    18576,
    18486,
    18395,
    18305,
    18215,
    18125,
    18035,
    17945,
    17856,
    17766,
    17677,
    17588,
    17499,
    17410,
    17321,
    17232,
    17144,
    17056,
    16968,
    16880,
    16792,
    16704,
    16616,
    16529,
    16442,
    16355,
    16268,
    16181,
    16094,
    16008,
    15922,
    15835,
    15750,
    15664,
    15578,
    15493,
    15407,
    15322,
    15237,
    15152,
    15067,
    14983,
    14899,
    14814,
    14730,
    14647,
    14563,
    14479,
    14396,
    14313,
    14230,
    14147,
    14064,
    13982,
    13900,
    13818,
    13736,
    13654,
    13572,
    13491,
    13410,
    13329,
    13248,
    13167,
    13087,
    13007,
    12926,
    12847,
    12767,
    12687,
    12608,
    12529,
    12450,
    12371,
    12292,
    12214,
    12136,
    12058,
    11980,
    11902,
    11825,
    11748,
    11671,
    11594,
    11517,
    11441,
    11365,
    11289,
    11213,
    11137,
    11062,
    10987,
    10912,
    10837,
    10762,
    10688,
    10614,
    10540,
    10466,
    10392,
    10319,
    10246,
    10173,
    10100,
    10028,
    9956,
    9883,
    9812,
    9740,
    9669,
    9597,
    9526,
    9456,
    9385,
    9315,
    9245,
    9175,
    9105,
    9036,
    8967,
    8898,
    8829,
    8760,
    8692,
    8624,
    8556,
    8488,
    8421,
    8354,
    8287,
    8220,
    8154,
    8087,
    8021,
    7956,
    7890,
    7825,
    7760,
    7695,
    7630,
    7566,
    7502,
    7438,
    7374,
    7311,
    7248,
    7185,
    7122,
    7060,
    6997,
    6935,
    6874,
    6812,
    6751,
    6690,
    6629,
    6569,
    6508,
    6448,
    6389,
    6329,
    6270,
    6211,
    6152,
    6094,
    6035,
    5977,
    5920,
    5862,
    5805,
    5748,
    5691,
    5635,
    5578,
    5522,
    5467,
    5411,
    5356,
    5301,
    5246,
    5192,
    5138,
    5084,
    5030,
    4977,
    4924,
    4871,
    4818,
    4766,
    4714,
    4662,
    4610,
    4559,
    4508,
    4457,
    4407,
    4357,
    4307,
    4257,
    4208,
    4158,
    4109,
    4061,
    4013,
    3964,
    3917,
    3869,
    3822,
    3775,
    3728,
    3682,
    3636,
    3590,
    3544,
    3499,
    3454,
    3409,
    3364,
    3320,
    3276,
    3233,
    3189,
    3146,
    3103,
    3061,
    3018,
    2976,
    2935,
    2893,
    2852,
    2811,
    2771,
    2730,
    2690,
    2650,
    2611,
    2572,
    2533,
    2494,
    2456,
    2418,
    2380,
    2343,
    2305,
    2269,
    2232,
    2196,
    2160,
    2124,
    2088,
    2053,
    2018,
    1984,
    1949,
    1915,
    1882,
    1848,
    1815,
    1782,
    1750,
    1717,
    1686,
    1654,
    1622,
    1591,
    1561,
    1530,
    1500,
    1470,
    1440,
    1411,
    1382,
    1353,
    1325,
    1297,
    1269,
    1241,
    1214,
    1187,
    1160,
    1134,
    1108,
    1082,
    1057,
    1031,
    1007,
    982,
    958,
    934,
    910,
    887,
    864,
    841,
    818,
    796,
    774,
    753,
    731,
    710,
    690,
    669,
    649,
    630,
    610,
    591,
    572,
    554,
    535,
    517,
    500,
    482,
    465,
    449,
    432,
    416,
    400,
    385,
    370,
    355,
    340,
    326,
    312,
    298,
    285,
    272,
    259,
    246,
    234,
    222,
    211,
    200,
    189,
    178,
    168,
    158,
    148,
    139,
    130,
    121,
    112,
    104,
    96,
    89,
    82,
    75,
    68,
    62,
    56,
    50,
    45,
    39,
    35,
    30,
    26,
    22,
    19,
    15,
    12,
    10,
    8,
    6,
    4,
    2,
    1,
    1,
    0,
    0,
    0,
    1,
    1,
    2,
    4,
    6,
    8,
    10,
    12,
    15,
    19,
    22,
    26,
    30,
    35,
    39,
    45,
    50,
    56,
    62,
    68,
    75,
    82,
    89,
    96,
    104,
    112,
    121,
    130,
    139,
    148,
    158,
    168,
    178,
    189,
    200,
    211,
    222,
    234,
    246,
    259,
    272,
    285,
    298,
    312,
    326,
    340,
    355,
    370,
    385,
    400,
    416,
    432,
    449,
    465,
    482,
    500,
    517,
    535,
    554,
    572,
    591,
    610,
    630,
    649,
    669,
    690,
    710,
    731,
    753,
    774,
    796,
    818,
    841,
    864,
    887,
    910,
    934,
    958,
    982,
    1007,
    1031,
    1057,
    1082,
    1108,
    1134,
    1160,
    1187,
    1214,
    1241,
    1269,
    1297,
    1325,
    1353,
    1382,
    1411,
    1440,
    1470,
    1500,
    1530,
    1561,
    1591,
    1622,
    1654,
    1686,
    1717,
    1750,
    1782,
    1815,
    1848,
    1882,
    1915,
    1949,
    1984,
    2018,
    2053,
    2088,
    2124,
    2160,
    2196,
    2232,
    2269,
    2305,
    2343,
    2380,
    2418,
    2456,
    2494,
    2533,
    2572,
    2611,
    2650,
    2690,
    2730,
    2771,
    2811,
    2852,
    2893,
    2935,
    2976,
    3018,
    3061,
    3103,
    3146,
    3189,
    3233,
    3276,
    3320,
    3364,
    3409,
    3454,
    3499,
    3544,
    3590,
    3636,
    3682,
    3728,
    3775,
    3822,
    3869,
    3917,
    3964,
    4013,
    4061,
    4109,
    4158,
    4208,
    4257,
    4307,
    4357,
    4407,
    4457,
    4508,
    4559,
    4610,
    4662,
    4714,
    4766,
    4818,
    4871,
    4924,
    4977,
    5030,
    5084,
    5138,
    5192,
    5246,
    5301,
    5356,
    5411,
    5467,
    5522,
    5578,
    5635,
    5691,
    5748,
    5805,
    5862,
    5920,
    5977,
    6035,
    6094,
    6152,
    6211,
    6270,
    6329,
    6389,
    6448,
    6508,
    6569,
    6629,
    6690,
    6751,
    6812,
    6874,
    6935,
    6997,
    7060,
    7122,
    7185,
    7248,
    7311,
    7374,
    7438,
    7502,
    7566,
    7630,
    7695,
    7760,
    7825,
    7890,
    7956,
    8021,
    8087,
    8154,
    8220,
    8287,
    8354,
    8421,
    8488,
    8556,
    8624,
    8692,
    8760,
    8829,
    8898,
    8967,
    9036,
    9105,
    9175,
    9245,
    9315,
    9385,
    9456,
    9526,
    9597,
    9669,
    9740,
    9812,
    9883,
    9956,
    10028,
    10100,
    10173,
    10246,
    10319,
    10392,
    10466,
    10540,
    10614,
    10688,
    10762,
    10837,
    10912,
    10987,
    11062,
    11137,
    11213,
    11289,
    11365,
    11441,
    11517,
    11594,
    11671,
    11748,
    11825,
    11902,
    11980,
    12058,
    12136,
    12214,
    12292,
    12371,
    12450,
    12529,
    12608,
    12687,
    12767,
    12847,
    12926,
    13007,
    13087,
    13167,
    13248,
    13329,
    13410,
    13491,
    13572,
    13654,
    13736,
    13818,
    13900,
    13982,
    14064,
    14147,
    14230,
    14313,
    14396,
    14479,
    14563,
    14647,
    14730,
    14814,
    14899,
    14983,
    15067,
    15152,
    15237,
    15322,
    15407,
    15493,
    15578,
    15664,
    15750,
    15835,
    15922,
    16008,
    16094,
    16181,
    16268,
    16355,
    16442,
    16529,
    16616,
    16704,
    16792,
    16880,
    16968,
    17056,
    17144,
    17232,
    17321,
    17410,
    17499,
    17588,
    17677,
    17766,
    17856,
    17945,
    18035,
    18125,
    18215,
    18305,
    18395,
    18486,
    18576,
    18667,
    18758,
    18849,
    18940,
    19031,
    19122,
    19214,
    19305,
    19397,
    19489,
    19581,
    19673,
    19765,
    19857,
    19950,
    20042,
    20135,
    20228,
    20321,
    20414,
    20507,
    20600,
    20694,
    20787,
    20881,
    20975,
    21068,
    21162,
    21257,
    21351,
    21445,
    21539,
    21634,
    21728,
    21823,
    21918,
    22013,
    22108,
    22203,
    22298,
    22394,
    22489,
    22584,
    22680,
    22776,
    22872,
    22967,
    23063,
    23159,
    23256,
    23352,
    23448,
    23545,
    23641,
    23738,
    23834,
    23931,
    24028,
    24125,
    24222,
    24319,
    24416,
    24513,
    24611,
    24708,
    24806,
    24903,
    25001,
    25099,
    25196,
    25294,
    25392,
    25490,
    25588,
    25686,
    25784,
    25883,
    25981,
    26079,
    26178,
    26276,
    26375,
    26474,
    26572,
    26671,
    26770,
    26869,
    26968,
    27066,
    27166,
    27265,
    27364,
    27463,
    27562,
    27661,
    27761,
    27860,
    27960,
    28059,
    28158,
    28258,
    28358,
    28457,
    28557,
    28657,
    28756,
    28856,
    28956,
    29056,
    29156,
    29256,
    29356,
    29456,
    29556,
    29656,
    29756,
    29856,
    29956,
    30056,
    30157,
    30257,
    30357,
    30457,
    30558,
    30658,
    30758,
    30859,
    30959,
    31059,
    31160,
    31260,
    31361,
    31461,
    31561,
    31662,
    31762,
    31863,
    31963,
    32064,
    32164,
    32265,
    32365,
    32466,
    32566,
    32667,
    32768,
    32868,
    32969,
    33069,
    33170,
    33270,
    33371,
    33471,
    33572,
    33672,
    33773,
    33873,
    33974,
    34074,
    34174,
    34275,
    34375,
    34476,
    34576,
    34676,
    34777,
    34877,
    34977,
    35078,
    35178,
    35278,
    35378,
    35479,
    35579,
    35679,
    35779,
    35879,
    35979,
    36079,
    36179,
    36279,
    36379,
    36479,
    36579,
    36679,
    36779,
    36878,
    36978,
    37078,
    37177,
    37277,
    37377,
    37476,
    37575,
    37675,
    37774,
    37874,
    37973,
    38072,
    38171,
    38270,
    38369,
    38469,
    38567,
    38666,
    38765,
    38864,
    38963,
    39061,
    39160,
    39259,
    39357,
    39456,
    39554,
    39652,
    39751,
    39849,
    39947,
    40045,
    40143,
    40241,
    40339,
    40436,
    40534,
    40632,
    40729,
    40827,
    40924,
    41022,
    41119,
    41216,
    41313,
    41410,
    41507,
    41604,
    41701,
    41797,
    41894,
    41990,
    42087,
    42183,
    42279,
    42376,
    42472,
    42568,
    42663,
    42759,
    42855,
    42951,
    43046,
    43141,
    43237,
    43332,
    43427,
    43522,
    43617,
    43712,
    43807,
    43901,
    43996,
    44090,
    44184,
    44278,
    44373,
    44467,
    44560,
    44654,
    44748,
    44841,
    44935,
    45028,
    45121,
    45214,
    45307,
    45400,
    45493,
    45585,
    45678,
    45770,
    45862,
    45954,
    46046,
    46138,
    46230,
    46321,
    46413,
    46504,
    46595,
    46686,
    46777,
    46868,
    46959,
    47049,
    47140,
    47230,
    47320,
    47410,
    47500,
    47590,
    47679,
    47769,
    47858,
    47947,
    48036,
    48125,
    48214,
    48303,
    48391,
    48479,
    48567,
    48655,
    48743,
    48831,
    48919,
    49006,
    49093,
    49180,
    49267,
    49354,
    49441,
    49527,
    49613,
    49700,
    49785,
    49871,
    49957,
    50042,
    50128,
    50213,
    50298,
    50383,
    50468,
    50552,
    50636,
    50721,
    50805,
    50888,
    50972,
    51056,
    51139,
    51222,
    51305,
    51388,
    51471,
    51553,
    51635,
    51717,
    51799,
    51881,
    51963,
    52044,
    52125,
    52206,
    52287,
    52368,
    52448,
    52528,
    52609,
    52688,
    52768,
    52848,
    52927,
    53006,
    53085,
    53164,
    53243,
    53321,
    53399,
    53477,
    53555,
    53633,
    53710,
    53787,
    53864,
    53941,
    54018,
    54094,
    54170,
    54246,
    54322,
    54398,
    54473,
    54548,
    54623,
    54698,
    54773,
    54847,
    54921,
    54995,
    55069,
    55143,
    55216,
    55289,
    55362,
    55435,
    55507,
    55579,
    55652,
    55723,
    55795,
    55866,
    55938,
    56009,
    56079,
    56150,
    56220,
    56290,
    56360,
    56430,
    56499,
    56568,
    56637,
    56706,
    56775,
    56843,
    56911,
    56979,
    57047,
    57114,
    57181,
    57248,
    57315,
    57381,
    57448,
    57514,
    57579,
    57645,
    57710,
    57775,
    57840,
    57905,
    57969,
    58033,
    58097,
    58161,
    58224,
    58287,
    58350,
    58413,
    58475,
    58538,
    58600,
    58661,
    58723,
    58784,
    58845,
    58906,
    58966,
    59027,
    59087,
    59146,
    59206,
    59265,
    59324,
    59383,
    59441,
    59500,
    59558,
    59615,
    59673,
    59730,
    59787,
    59844,
    59900,
    59957,
    60013,
    60068,
    60124,
    60179,
    60234,
    60289,
    60343,
    60397,
    60451,
    60505,
    60558,
    60611,
    60664,
    60717,
    60769,
    60821,
    60873,
    60925,
    60976,
    61027,
    61078,
    61128,
    61178,
    61228,
    61278,
    61327,
    61377,
    61426,
    61474,
    61522,
    61571,
    61618,
    61666,
    61713,
    61760,
    61807,
    61853,
    61899,
    61945,
    61991,
    62036,
    62081,
    62126,
    62171,
    62215,
    62259,
    62302,
    62346,
    62389,
    62432,
    62474,
    62517,
    62559,
    62600,
    62642,
    62683,
    62724,
    62764,
    62805,
    62845,
    62885,
    62924,
    62963,
    63002,
    63041,
    63079,
    63117,
    63155,
    63192,
    63230,
    63266,
    63303,
    63339,
    63375,
    63411,
    63447,
    63482,
    63517,
    63551,
    63586,
    63620,
    63653,
    63687,
    63720,
    63753,
    63785,
    63818,
    63849,
    63881,
    63913,
    63944,
    63974,
    64005,
    64035,
    64065,
    64095,
    64124,
    64153,
    64182,
    64210,
    64238,
    64266,
    64294,
    64321,
    64348,
    64375,
    64401,
    64427,
    64453,
    64478,
    64504,
    64528,
    64553,
    64577,
    64601,
    64625,
    64648,
    64671,
    64694,
    64717,
    64739,
    64761,
    64782,
    64804,
    64825,
    64845,
    64866,
    64886,
    64905,
    64925,
    64944,
    64963,
    64981,
    65000,
    65018,
    65035,
    65053,
    65070,
    65086,
    65103,
    65119,
    65135,
    65150,
    65165,
    65180,
    65195,
    65209,
    65223,
    65237,
    65250,
    65263,
    65276,
    65289,
    65301,
    65313,
    65324,
    65335,
    65346,
    65357,
    65367,
    65377,
    65387,
    65396,
    65405,
    65414,
    65423,
    65431,
    65439,
    65446,
    65453,
    65460,
    65467,
    65473,
    65479,
    65485,
    65490,
    65496,
    65500,
    65505,
    65509,
    65513,
    65516,
    65520,
    65523,
    65525,
    65527,
    65529,
    65531,
    65533,
    65534,
    65534,
    65535,
    65535,
    65535,
    65534,
    65534,
    65533,
    65531,
    65529,
    65527,
    65525,
    65523,
    65520,
    65516,
    65513,
    65509,
    65505,
    65500,
    65496,
    65490,
    65485,
    65479,
    65473,
    65467,
    65460,
    65453,
    65446,
    65439,
    65431,
    65423,
    65414,
    65405,
    65396,
    65387,
    65377,
    65367,
    65357,
    65346,
    65335,
    65324,
    65313,
    65301,
    65289,
    65276,
    65263,
    65250,
    65237,
    65223,
    65209,
    65195,
    65180,
    65165,
    65150,
    65135,
    65119,
    65103,
    65086,
    65070,
    65053,
    65035,
    65018,
    65000,
    64981,
    64963,
    64944,
    64925,
    64905,
    64886,
    64866,
    64845,
    64825,
    64804,
    64782,
    64761,
    64739,
    64717,
    64694,
    64671,
    64648,
    64625,
    64601,
    64577,
    64553,
    64528,
    64504,
    64478,
    64453,
    64427,
    64401,
    64375,
    64348,
    64321,
    64294,
    64266,
    64238,
    64210,
    64182,
    64153,
    64124,
    64095,
    64065,
    64035,
    64005,
    63974,
    63944,
    63913,
    63881,
    63849,
    63818,
    63785,
    63753,
    63720,
    63687,
    63653,
    63620,
    63586,
    63551,
    63517,
    63482,
    63447,
    63411,
    63375,
    63339,
    63303,
    63266,
    63230,
    63192,
    63155,
    63117,
    63079,
    63041,
    63002,
    62963,
    62924,
    62885,
    62845,
    62805,
    62764,
    62724,
    62683,
    62642,
    62600,
    62559,
    62517,
    62474,
    62432,
    62389,
    62346,
    62302,
    62259,
    62215,
    62171,
    62126,
    62081,
    62036,
    61991,
    61945,
    61899,
    61853,
    61807,
    61760,
    61713,
    61666,
    61618,
    61571,
    61522,
    61474,
    61426,
    61377,
    61327,
    61278,
    61228,
    61178,
    61128,
    61078,
    61027,
    60976,
    60925,
    60873,
    60821,
    60769,
    60717,
    60664,
    60611,
    60558,
    60505,
    60451,
    60397,
    60343,
    60289,
    60234,
    60179,
    60124,
    60068,
    60013,
    59957,
    59900,
    59844,
    59787,
    59730,
    59673,
    59615,
    59558,
    59500,
    59441,
    59383,
    59324,
    59265,
    59206,
    59146,
    59087,
    59027,
    58966,
    58906,
    58845,
    58784,
    58723,
    58661,
    58600,
    58538,
    58475,
    58413,
    58350,
    58287,
    58224,
    58161,
    58097,
    58033,
    57969,
    57905,
    57840,
    57775,
    57710,
    57645,
    57579,
    57514,
    57448,
    57381,
    57315,
    57248,
    57181,
    57114,
    57047,
    56979,
    56911,
    56843,
    56775,
    56706,
    56637,
    56568,
    56499,
    56430,
    56360,
    56290,
    56220,
    56150,
    56079,
    56009,
    55938,
    55866,
    55795,
    55723,
    55652,
    55579,
    55507,
    55435,
    55362,
    55289,
    55216,
    55143,
    55069,
    54995,
    54921,
    54847,
    54773,
    54698,
    54623,
    54548,
    54473,
    54398,
    54322,
    54246,
    54170,
    54094,
    54018,
    53941,
    53864,
    53787,
    53710,
    53633,
    53555,
    53477,
    53399,
    53321,
    53243,
    53164,
    53085,
    53006,
    52927,
    52848,
    52768,
    52688,
    52609,
    52528,
    52448,
    52368,
    52287,
    52206,
    52125,
    52044,
    51963,
    51881,
    51799,
    51717,
    51635,
    51553,
    51471,
    51388,
    51305,
    51222,
    51139,
    51056,
    50972,
    50888,
    50805,
    50721,
    50636,
    50552,
    50468,
    50383,
    50298,
    50213,
    50128,
    50042,
    49957,
    49871,
    49785,
    49700,
    49613,
    49527,
    49441,
    49354,
    49267,
    49180,
    49093,
    49006,
    48919,
    48831,
    48743,
    48655,
    48567,
    48479,
    48391,
    48303,
    48214,
    48125,
    48036,
    47947,
    47858,
    47769,
    47679,
    47590,
    47500,
    47410,
    47320,
    47230,
    47140,
    47049,
    46959,
    46868,
    46777,
    46686,
    46595,
    46504,
    46413,
    46321,
    46230,
    46138,
    46046,
    45954,
    45862,
    45770,
    45678,
    45585,
    45493,
    45400,
    45307,
    45214,
    45121,
    45028,
    44935,
    44841,
    44748,
    44654,
    44560,
    44467,
    44373,
    44278,
    44184,
    44090,
    43996,
    43901,
    43807,
    43712,
    43617,
    43522,
    43427,
    43332,
    43237,
    43141,
    43046,
    42951,
    42855,
    42759,
    42663,
    42568,
    42472,
    42376,
    42279,
    42183,
    42087,
    41990,
    41894,
    41797,
    41701,
    41604,
    41507,
    41410,
    41313,
    41216,
    41119,
    41022,
    40924,
    40827,
    40729,
    40632,
    40534,
    40436,
    40339,
    40241,
    40143,
    40045,
    39947,
    39849,
    39751,
    39652,
    39554,
    39456,
    39357,
    39259,
    39160,
    39061,
    38963,
    38864,
    38765,
    38666,
    38567,
    38469,
    38369,
    38270,
    38171,
    38072,
    37973,
    37874,
    37774,
    37675,
    37575,
    37476,
    37377,
    37277,
    37177,
    37078,
    36978,
    36878,
    36779,
    36679,
    36579,
    36479,
    36379,
    36279,
    36179,
    36079,
    35979,
    35879,
    35779,
    35679,
    35579,
    35479,
    35378,
    35278,
    35178,
    35078,
    34977,
    34877,
    34777,
    34676,
    34576,
    34476,
    34375,
    34275,
    34174,
    34074,
    33974,
    33873,
    33773,
    33672,
    33572,
    33471,
    33371,
    33270,
    33170,
    33069,
    32969,
    32868
};

#endif