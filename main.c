#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <math.h>
#include <string.h>
#include <locale.h>

typedef char* c_str;

typedef unsigned long long uint64_t;
typedef unsigned int uint32_t;
typedef uint64_t size_t;

typedef struct {
    uint32_t day; //2digits
    uint32_t month; //2digits
    uint32_t year; //2digits
    uint32_t ordinalNumber; //3digits
    uint32_t genderDigit; //1digit
    uint32_t controlDigit; //1digit
} pesel_t;

typedef struct {
    size_t index;
    pesel_t pesel;
} pesel_index_t;

typedef struct {
    pesel_index_t* vec;
    size_t size;
    size_t len;
} pesel_vector_t;

inline bool isEven(const uint32_t number) {
    return number % 2 == 0;
}

inline bool isSameDay(const pesel_t* _val1, const pesel_t* _val2) {
    return _val1->day == _val2->day && _val1->month == _val2->month && _val1->year == _val2->year;
}

inline pesel_index_t parsePesel(const c_str pesel_str) {
    pesel_index_t pesel;
    sscanf_s(pesel_str, "%zu %02u%02u%02u%03u%01u%01u",
           &pesel.index,
           &pesel.pesel.year,
           &pesel.pesel.month,
           &pesel.pesel.day,
           &pesel.pesel.ordinalNumber,
           &pesel.pesel.genderDigit,
           &pesel.pesel.controlDigit);
    if (pesel.pesel.month > 60) {
        pesel.pesel.month -= 60;
        pesel.pesel.year += 2200;
    } else if (pesel.pesel.month > 40) {
        pesel.pesel.month -= 40;
        pesel.pesel.year += 2100;
    } else if (pesel.pesel.month > 20) {
        pesel.pesel.month -= 20;
        pesel.pesel.year += 2000;
    }
    return pesel;
}

inline c_str stringifyPesel(const pesel_index_t* pesel) {
    static char buffer[16] = {};
    uint32_t monthOffset = 0u;
    if (pesel->pesel.year >= 2200) monthOffset=60u;
    else if (pesel->pesel.year >= 2100) monthOffset=40u;
    else if (pesel->pesel.year >= 2000) monthOffset=20u;
    snprintf(buffer, sizeof(buffer), "%zu %02u%02u%02u%03u%01u%01u", pesel->index,
        pesel->pesel.year%100u, pesel->pesel.month+monthOffset, pesel->pesel.day, pesel->pesel.ordinalNumber, pesel->pesel.genderDigit, pesel->pesel.controlDigit);
    return buffer;
}

inline bool isOlder(const pesel_t* _val1, const pesel_t* _val2) {
    return _val1->year > _val2->year || (_val1->year == _val2->year && _val1->month > _val2->month) || (_val1->year == _val2->year && _val1->month == _val2->month && _val1->day > _val2->day);
}

inline bool isSameAge(const pesel_t* _val1, const pesel_t* _val2) {
    return _val1->year == _val2->year && _val1->month == _val2->month && _val1->day == _val2->day;
}

inline bool isFemale(const pesel_t* _val) {
    return isEven(_val->genderDigit);
}

inline int genderCompare(const pesel_t* _val1, const pesel_t* _val2) {
    const auto af = isFemale(_val1);
    const auto bf = isFemale(_val2);
    if (af == bf) return 0;
    if (af) return -1;
    return 1;
}

int order(const pesel_index_t* _val1, const pesel_index_t* _val2) {
    if (isOlder(&_val1->pesel, &_val2->pesel)) return 1;
    if (isSameAge(&_val1->pesel, &_val2->pesel)) {
        auto rs =  genderCompare(&_val1->pesel, &_val2->pesel);
        if (rs == 0) return _val1->index - _val2->index; //stabilize sorting
        return rs;
    }
    return -1;
}

inline uint32_t computeOrdinalNumberAndGenderDigit(const pesel_t* pesel, const pesel_vector_t* others, const bool is_female) {
    uint32_t sameGenderOnSameDay = 0u;
    for (size_t i = 0; i < others->len; i++) {
        if (isSameDay(pesel, &others->vec[i].pesel) && is_female == isFemale(&others->vec[i].pesel)) ++sameGenderOnSameDay;
    }
    sameGenderOnSameDay*=2;
    if (!is_female) {
        sameGenderOnSameDay+=1;
    }

    if (sameGenderOnSameDay >= 10000) {
        printf("Too many same gender people on same day");
        exit(1);
    }

    return sameGenderOnSameDay;
}


inline uint32_t computeControlDigit(const pesel_t* pesel) {
    uint64_t sum = 0u;
    const static uint32_t weights[] = { 1, 3, 7, 9, 1, 3, 7, 9, 1, 3};
    sum += pesel->day/10 * weights[0];
    sum += pesel->day%10 * weights[1];
    sum += pesel->month/10 * weights[2];
    sum += pesel->month%10 * weights[3];
    sum += pesel->year/10 * weights[4];
    sum += pesel->year%10 * weights[5];
    sum += pesel->ordinalNumber/100 * weights[6];
    sum += pesel->ordinalNumber/10%10 * weights[7];
    sum += pesel->ordinalNumber%10 * weights[8];
    sum += pesel->genderDigit * weights[9];
    return sum%10;
}

inline bool chrCompareIgnoreCase(const char a, const char b) {
    return tolower(a) == tolower(b);
}

bool strCompareIgnoreCase(const c_str a, const c_str b, const size_t len) {
    for (size_t i = 0; i < len; i++) {
        if (!chrCompareIgnoreCase(a[i], b[i])) {
            return false;
        }
    }
    return true;
}

inline bool checkButtonPress(const char t) {
    char c;
    scanf_s(" %c", &c, 1);
    return c == t;
}

inline bool pToContinue() {
    return checkButtonPress('p');
}

inline uint32_t parseNumericDate() {;
    uint32_t date = 0;
    scanf_s("%u", &date);
    return date;
}

uint32_t getDay(uint32_t daysInMonth) {
    printf("Podaj numer dnia miesiąca urodzenia, np. 23.\n");
    auto day = parseNumericDate();
    if (day < 1 || day > daysInMonth) {
        printf("Podałeś zły numer, wciśnij p jeśli chcesz podać ponownie numer dnia lub wciśnij inny klawisz jeśli chcesz zakończyć\n");
        if (pToContinue()) {
            return getDay(daysInMonth);
        }else {
            exit(0);
        }
    }
    return day;
}

uint32_t getMonth() {
    const static c_str monthNames[] = {"none", "styczeń", "luty", "marzec", "kwiecień", "maj", "czerwiec", "lipiec", "sierpień", "wrzesień", "październik", "listopad", "grudzień"};
    printf("Podaj miesiąc urodzenia, np. czerwiec albo 6.\n");
    char buffer[15] = {};
    uint32_t month;
    scanf_s("%s", buffer, sizeof(buffer));
    if (isdigit(buffer[0])) {
        month = atoi(buffer);
    }else {
        for (month = 1; month <= 12; month++) {
            if (strCompareIgnoreCase(monthNames[month], buffer, min(strlen(monthNames[month]),  sizeof(buffer)))) {
                break;
            }
            if (month == 12) {
                month = 20;
            }
        }

    }
    if (month < 1 || month > 12) {
        printf("Podałeś złą formę miesiąca, wciśnij p jeśli chcesz podać ponownie miesiąc lub wciśnij inny klawisz jeśli chcesz zakończyć\n");
        if (pToContinue()) {
            return getMonth();
        }else {
            exit(0);
        }
    }
    return month;
}

uint32_t getYear() {
    printf("Podaj czterocyfrowy rok urodzenia, np. 1993.\n");
    const uint32_t year = parseNumericDate();
    if (year < 1900 || year > 2299) {
        printf("Podałeś złą formę roku, wciśnij p jeśli chcesz podać ponownie rok lub wciśnij inny klawisz jeśli chcesz zakończyć\n");
        if (pToContinue()) {
            return getYear();
        }else {
            exit(0);
        }
    }
    return year;
}


bool isMale() {
    printf("Podaj płeć: wciśnij k jeśli jesteś kobietą albo m jeśli jesteś mężczyzną.\n");
    char gender;
    scanf_s(" %c", &gender, 1);
    if (gender == 'k') {
        return false;
    }else if (gender == 'm') {
        return true;
    }else {
        printf("Podałeś niepoprawną formę płci, wciśnij p jeśli chcesz podać ponownie płeć lub wciśnij inny klawisz jeśli chcesz zakończyć.\n");
        if (pToContinue()) {
            return isMale();
        }else {
            exit(0);
        }
    }
}

inline bool verifyData(const pesel_t* pesel) {
    printf("Czy chcesz dokonać wpisu %4u, %u, %u, %c? Klawisz t – tak, pozostałe – nie.\n", pesel->year,  pesel->month,  pesel->day, isEven(pesel->genderDigit) ? 'k' : 'm');
    if (checkButtonPress('t')) return true;
    return false;
}

bool anotherOne() {
    printf("Czy chcesz dokonać kolejnego wpisu? Klawisz t – tak, pozostałe – nie.\n");
    if (checkButtonPress('t')) return true;
    return false;
}

pesel_vector_t createVector(const size_t size) {
    if (size == 0) {
        printf("Can't create vector of size 0");
        exit(1);
    }
    pesel_vector_t vec;
    vec.len = 0;
    vec.size = size;
    vec.vec = malloc(sizeof(pesel_index_t)*size);
    if (vec.vec == NULL) {
        printf("Failed to allocate memory");
        exit(1);
    }
    return vec;
}

void freeVector(pesel_vector_t vec) {
    free(vec.vec);
    vec.len = 0;
    vec.size = 0;
}

void append(pesel_vector_t* vec, const pesel_index_t* pesel) {
    if (vec->size == 0) {
        printf("Tried appending to freed vector");
        exit(1);
    }
    if (vec->size == vec->len) {
        vec->size = (size_t)ceil(vec->size*2);
        vec->vec = realloc(vec->vec, sizeof(pesel_index_t)*vec->size);
        if (vec->vec == NULL) {
            printf("Failed to allocate memory");
            exit(1);
        }
    }
    vec->vec[vec->len++] = *pesel;
}

void shrink(pesel_vector_t* vec, const size_t desiredSize) {
    if (desiredSize == 0) {
        printf("Tried shrinking to freed vector");
        exit(1);
    }
    if (desiredSize > vec->size) {
        printf("Tried shrinking to bigger vector");
        exit(1);
    }

    if (desiredSize == vec->size) return; // No need to shrink
    vec->size = desiredSize;
    vec->vec = realloc(vec->vec, sizeof(pesel_index_t)*vec->size);
    if (vec->vec == NULL) {
        printf("Failed to allocate memory");
        exit(1);
    }
}

void pop(pesel_vector_t* vec) {
    if (vec->len == 0) {
        printf("Tried popping from freed vector");
        exit(1);
    }
    if (vec->len < vec->size/2) {
        shrink(vec, (size_t)ceil(vec->size*1.3));
    }
    vec->len--;
}

uint32_t daysInMonth(const uint32_t month, const uint32_t year) {
    if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
        return 31;
    }else if (month == 4 || month == 6 || month == 9 || month == 11) {
        return 30;
    }else {
        if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0)) {
            return 29;
        }
        return 28;
    }
}

bool validatePesel(const pesel_t* pesel) {
    if (pesel->year < 1900 || pesel->year >= 2300) return false;
    if (pesel->month < 1 || pesel->month > 12) return false;
    if (pesel->day < 1 || pesel->day > daysInMonth(pesel->month, pesel->year)) return false;
    if (computeControlDigit(pesel) != pesel->controlDigit) return false;
    return true;
}

pesel_vector_t readData(const c_str file) {
    pesel_vector_t vec = createVector(3);
    FILE* f;
    fopen_s(&f, file, "r");
    if (f == NULL) {
        // File does not exist or permission denied
        return vec;
    }
    while (!feof(f)) {
        char buffer[16] = {};
        fgets(buffer, sizeof(buffer), f);
        pesel_index_t pesel = parsePesel(buffer);
        if (validatePesel(&pesel.pesel)) {
            append(&vec, &pesel);
        }
    }
    fclose(f);
    return vec;
}

void writeData(const pesel_vector_t* vec, const c_str file) {
    FILE* f;
    fopen_s(&f, file, "w");
    if (f == NULL) {
        printf("Failed to open file in write mode");
        exit(1);
    }
    for (size_t i = 0; i < vec->len; i++) {
        fprintf(f, "%s\n", stringifyPesel(&vec->vec[i]));
    }
    fclose(f);
}

int main(void) {
    if (setlocale(LC_ALL, ".utf-8") == NULL) {
        puts("Unable to set locale");
    }
    pesel_vector_t pesels = readData("pesel.txt");
    bool ctn = false;
    do {
        pesel_index_t newPesel = {pesels.len};
        newPesel.pesel.year = getYear();
        newPesel.pesel.month = getMonth();
        newPesel.pesel.day = getDay(daysInMonth(newPesel.pesel.month, newPesel.pesel.year));
        newPesel.pesel.ordinalNumber = computeOrdinalNumberAndGenderDigit(&newPesel.pesel, &pesels, !isMale());
        newPesel.pesel.genderDigit = newPesel.pesel.ordinalNumber%10;
        newPesel.pesel.ordinalNumber /= 10;
        newPesel.pesel.controlDigit = computeControlDigit(&newPesel.pesel);
        if (verifyData(&newPesel.pesel)) {
            append(&pesels, &newPesel);
        }
        qsort(pesels.vec, pesels.len, sizeof(pesel_index_t), order);
        writeData(&pesels, "pesel.txt");
        ctn = anotherOne();
    }while(ctn);
    freeVector(pesels);
}
