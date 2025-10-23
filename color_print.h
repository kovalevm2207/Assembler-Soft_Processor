#ifndef COLOR_PRINT
#define COLOR_PRINT

// fast color change

#define RED_COLOR          "\033[31m"
#define GREEN_COLOR        "\033[32m"
#define ORANGE_COLOR       "\033[33m"
#define BLUE_COLOR         "\033[34m"
#define PURPLE_COLOR       "\033[35m"
#define CYAN_COLOR         "\033[36m"

// command

#define  CHANGE_ON         "\033["
#define  WITH              ";"
#define  TEXT_COLOR        "m"
#define  RESET             "\033[0m"

// tipe of text

#define  NORMAL            "00"
#define  BOLD              "01"
#define  UNDERLINED        "04"
#define  BLINKING          "05"
#define  REVERSED          "07"
#define  CONCEALED         "08"

// color of text

#define  BLACK             "30"
#define  RED               "31"
#define  GREEN             "32"
#define  ORANGE            "33"
#define  BLUE              "34"
#define  PURPLE            "35"
#define  CYAN              "36"
#define  GREY              "37"
#define  DARK_GREY         "90"
#define  LIGHT_RED         "91"
#define  LIGHT_GREEN       "92"
#define  YELLOW            "93"
#define  LIGHT_BLUE        "94"
#define  LIGHT_PURPLE      "95"
#define  TURQUOISE         "96"

// color of background

#define  BLACK_BACK        "40"
#define  RED_BACK          "41"
#define  GREEN_BACK        "42"
#define  ORANGE_BACK       "43"
#define  BLUE_BACK         "44"
#define  PURPLET_BACK      "45"
#define  CYAN_BACK         "46"
#define  GREY_BACK         "47"
#define  DARK_GREY_BACK    "100"
#define  LIGHT_RED_BACK    "101"
#define  LIGHT_GREEN_BACK  "102"
#define  YELLOW_BACK       "103"
#define  LIGHT_BLUE_BACK   "104"
#define  LIGHT_PURPLE_BACK "105"
#define  TURCUOISE_BACK    "106"

#endif // COLOR_PRINT
