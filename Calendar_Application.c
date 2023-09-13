#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include <time.h>
const char* months[] = {"January", "February", "March", "April", "May", "June", "July",
                        "August", "September", "October", "November", "December"};

struct event {
    int day;
    int month;
    int year;
    char title[50];
};

void gotoxy(int, int);
void menu();
void add_event();
void view_calendar();
void modify_event();
void search_event();
void delete_event();
void list_events();
unsigned int days_in_month(unsigned int month, unsigned int year);
int is_leap_year(int year);

void print_centered_text(const char* text) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int screenWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    int textLength = strlen(text);
    int xOffset = (screenWidth - textLength) / 2;

    for (int i = 0; i < xOffset; i++) {
        printf(" ");
    }
    printf("%s\n", text);
}


int main() {
    menu();

    return 0;
}

void menu() {
    int choice;
    do {
        system("cls");
        print_centered_text("<--:CALENDAR APPLICATION:-->\n");
        printf("\n");
        gotoxy(10,2);
        printf("<--:Menu:-->\n");
        printf("\n");
        gotoxy(10,4);
        printf("1 : Add Event\n");
        gotoxy(10,5);
        printf("2 : View Calendar\n");
        gotoxy(10,6);
        printf("3 : Modify Event\n");
        gotoxy(10,7);
        printf("4 : Search Event\n");
        gotoxy(10,8);
        printf("5 : Delete Event\n");
        gotoxy(10,9);
        printf("6 : List Events\n");
        gotoxy(10,10);
        printf("7 : Exit\n\n");
        printf(" ");
        gotoxy(10,11);
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                add_event();
                break;
            case 2:
                view_calendar();
                break;
            case 3:
                modify_event();
                break;
            case 4:
                search_event();
                break;
            case 5:
                delete_event();
                break;
            case 6:
                list_events();
                break;
            case 7:
                exit(0);
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }

        print_centered_text("\nPress any key to continue...");
        getch();
    } while (1);
}

void add_event() {
    FILE *fp;
    struct event e;

    system("cls");
    print_centered_text("<--:ADD EVENT:-->\n");
    gotoxy(5,3);
    printf("Enter event day, month, year (DD MM YYYY): ");
    scanf("%d %d %d", &e.day, &e.month, &e.year);
    gotoxy(5,5);
    printf("Enter event title: ");
    fflush(stdin);
    gets(e.title);

    fp = fopen("events.txt", "ab");
    if (fp == NULL) {
        printf("Error opening file for writing.\n");
        return;
    }

    fwrite(&e, sizeof(struct event), 1, fp);
    fclose(fp);
    gotoxy(5,7);
    printf("Event added successfully!\n");
}

void view_calendar() {
    time_t t;
    struct tm* now;
    struct event e;

    system("cls");
    print_centered_text("<--:VIEW CALENDAR:-->\n");
    time(&t);
    now = localtime(&t);

    int currentYear = now->tm_year + 1900;
    int currentMonth = now->tm_mon + 1;

    int daysInMonth = days_in_month(currentMonth, currentYear);
    int weekday = now->tm_wday;
    int day = 1;
    printf("     %s %d\n", months[currentMonth - 1], currentYear);
    printf("Sun Mon Tue Wed Thu Fri Sat\n");

    // Initialize a 2D array to store events for each day of the month
    char eventTitles[31][50];
    for (int i = 0; i < 31; i++) {
        strcpy(eventTitles[i], "");
    }

    FILE* fp = fopen("events.txt", "rb");
    while (fread(&e, sizeof(struct event), 1, fp) == 1) {
        if (e.month == currentMonth && e.year == currentYear) {
            strcpy(eventTitles[e.day - 1], e.title);
        }
    }
    fclose(fp);

    for (int week = 0; week < 6; week++) {
        for (int wday = 0; wday < 7; wday++) {
            if ((week == 0 && wday < weekday) || day > daysInMonth) {
                printf("    ");
            } else {
                int isEventDay = (strcmp(eventTitles[day - 1], "") != 0);

                if (isEventDay) {
                    printf("\033[1;32m%3d\033[0m ", day);
                } else {
                    printf("%3d ", day);
                }
                day++;
            }
        }
        printf("\n");
        if (day > daysInMonth) {
            break;
        }
    }

    printf("\nEvent Titles:\n");
    for (int i = 0; i < daysInMonth; i++) {
        if (strcmp(eventTitles[i], "") != 0) {
            printf("%2d: %s\n", i + 1, eventTitles[i]);
        }
    }
}



unsigned int days_in_month(unsigned int month, unsigned int year) {
    unsigned int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    int days = daysInMonth[month];
    if (month == 2 && is_leap_year(year)) {
        days++;
    }
    return days;
}

int is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}
void modify_event() {
    struct event e;
    int dayToModify;
    int eventFound = 0;

    system("cls");
    print_centered_text("<--:MODIFY EVENT:-->\n");

    printf("Enter the day of the event to modify: ");
    scanf("%d", &dayToModify);

    FILE* fp = fopen("events.txt", "rb+");
    if (fp == NULL) {
        printf("Error opening file for reading/writing.\n");
        return;
    }

    while (fread(&e, sizeof(struct event), 1, fp) == 1) {
        if (e.day == dayToModify) {
            eventFound = 1;
            printf("Event found!\n");
            printf("Enter new title for the event: ");
            fflush(stdin);
            gets(e.title);
            fseek(fp, (long)(-sizeof(struct event)), SEEK_CUR);
            fwrite(&e, sizeof(struct event), 1, fp);
            printf("Event title modified successfully!\n");
            break;
        }
    }

    fclose(fp);

    if (!eventFound) {
        printf("Event not found for the specified day.\n");
    }
}

void search_event() {
    struct event e;
    int dayToSearch, monthToSearch;
    int eventFound = 0;

    system("cls");
    print_centered_text("<--:SEARCH EVENT:-->\n");

    printf("Enter the day of the event to search: ");
    scanf("%d", &dayToSearch);

    printf("Enter the month of the event (1-12, or 0 if not known): ");
    scanf("%d", &monthToSearch);

    FILE* fp = fopen("events.txt", "rb");
    if (fp == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }

    while (fread(&e, sizeof(struct event), 1, fp) == 1) {
        if (monthToSearch == 0) {
            if (e.day == dayToSearch) {
                eventFound = 1;
                printf("Event found!\n");
                printf("Event details:\n");
                printf("Date: %d %s %d\n", e.day, months[e.month - 1], e.year);
                printf("Title: %s\n", e.title);
                break;
            }
        } else {
            if (e.day == dayToSearch && e.month == monthToSearch) {
                eventFound = 1;
                printf("Event found!\n");
                printf("Event details:\n");
                printf("Date: %d %s %d\n", e.day, months[e.month - 1], e.year);
                printf("Title: %s\n", e.title);
                break;
            }
        }
    }

    fclose(fp);

    if (!eventFound) {
        printf("Event not found for the specified day and month.\n");
    }
}
void delete_event() {
    struct event e;
    int dayToDelete, monthToDelete;
    int eventFound = 0;

    system("cls");
    print_centered_text("<--:DELETE EVENT:-->\n");

    printf("Enter the day of the event to delete: ");
    scanf("%d", &dayToDelete);

    printf("Enter the month of the event (1-12): ");
    scanf("%d", &monthToDelete);

    FILE* fp = fopen("events.txt", "rb");
    if (fp == NULL) {
        printf("Error opening file for reading.\n");
        return;
    }

    FILE* tempFile = fopen("temp.txt", "wb");
    if (tempFile == NULL) {
        printf("Error creating temporary file.\n");
        fclose(fp);
        return;
    }

    while (fread(&e, sizeof(struct event), 1, fp) == 1) {
        if (e.day == dayToDelete && e.month == monthToDelete) {
            eventFound = 1;
            printf("Event found and deleted!\n");
        } else {
            fwrite(&e, sizeof(struct event), 1, tempFile);
        }
    }

    fclose(fp);
    fclose(tempFile);

    remove("events.txt");
    rename("temp.txt", "events.txt");

    if (!eventFound) {
        printf("Event not found for the specified day and month.\n");
    }
}

void list_events() {
    struct event e;

    system("cls");
    print_centered_text("<--:LIST EVENTS:-->\n");

    FILE* fp = fopen("events.txt", "rb");
    if (fp == NULL) {
        print_centered_text("Error opening file for reading.\n");
        return;
    }

    printf("Event List:\n");
    printf("Day   Month    Year    Title\n");
    printf("===============================\n");

    while (fread(&e, sizeof(struct event), 1, fp) == 1) {
        printf("%-6d%-10s%-7d%s\n", e.day, months[e.month - 1], e.year, e.title);
    }

    fclose(fp);
    printf("===============================\n");
}


void gotoxy(int x, int y) {
    COORD c;
    c.X = x;
    c.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), c);
}
