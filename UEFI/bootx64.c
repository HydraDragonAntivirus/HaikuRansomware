#include <efi.h>
#include <efilib.h>

// UEFI color constants
#define EFI_BLACK        0x00
#define EFI_BLUE         0x01
#define EFI_GREEN        0x02
#define EFI_RED          0x04
#define EFI_BRIGHT       0x08

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifndef DEG_TO_RAD
#define DEG_TO_RAD(angle) ((angle) * M_PI / 180.0)
#endif

#define SPEAKER_PORT 0x61
#define TIMER_COMMAND_PORT 0x43
#define TIMER_DATA_PORT 0x42
#define PIT_FREQUENCY 1193180

// Custom absolute value function for double
double fabs(double x) {
    return (x < 0) ? -x : x;
}

static inline void outb(UINT16 port, UINT8 val) {
    __asm__ __volatile__ ("outb %0, %1" : : "a"(val), "Nd"(port));
}

static inline UINT8 inb(UINT16 port) {
    UINT8 ret;
    __asm__ __volatile__ ("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable);

INTN Sin(UINTN angle) {
    // Convert angle to radians
    double rad = DEG_TO_RAD(angle);

    // Taylor series approximation for sin(x)
    double term = rad;  // first term is x
    double sum = 0.0;
    int n = 1;

    while (fabs(term) > 1e-10) {  // convergence threshold
        sum += term;
        n += 2;
        term *= -rad * rad / ((n - 1) * n);
    }

    return (INTN)(sum * 10);  // scale the amplitude appropriately
}

// Custom random number generator
static unsigned long seed = 12345;
unsigned int my_rand() {
    seed = (seed * 1103515245 + 12345) % (1 << 31);
    return seed;
}

// Function for busy-wait delay
void delay(UINTN duration) {
    volatile UINTN i;
    for (i = 0; i < duration; i++);
}

// Function to generate random valid UEFI color (restrict certain combinations)
UINTN random_color() {
    // Define color codes manually
    UINTN fg_colors[16] = {
        EFI_BLACK, EFI_BLUE, EFI_GREEN, EFI_RED,
        EFI_BLACK | EFI_BRIGHT, EFI_BLUE | EFI_BRIGHT, EFI_GREEN | EFI_BRIGHT, EFI_RED | EFI_BRIGHT,
        EFI_BLACK | EFI_GREEN, EFI_BLACK | EFI_BLUE, EFI_BLACK | EFI_RED, EFI_BLACK | EFI_YELLOW,
        EFI_BLACK | EFI_MAGENTA, EFI_BLACK | EFI_CYAN, EFI_BLACK | EFI_WHITE, EFI_WHITE
    };

    // Randomly choose a foreground color from 16 colors
    UINTN fg_color = fg_colors[my_rand() % 16];
    // Randomly choose a background color from 16 colors
    UINTN bg_color = fg_colors[my_rand() % 16];

    // Ensure that the foreground and background aren't the same
    if (fg_color == bg_color) {
        fg_color = fg_colors[(my_rand() % 15) + 1];  // Shift the foreground if they are the same
    }

    // Combine background and foreground colors
    return (bg_color << 4) | fg_color;
}

// Effect 1: Rotating Characters with All Colors
void rotating_characters_all_colors(EFI_SYSTEM_TABLE* SystemTable) {
    EFI_SIMPLE_TEXT_OUT_PROTOCOL* ConOut = SystemTable->ConOut;
    UINTN Columns, Rows;
    EFI_STATUS Status;

    // Get the dimensions of the screen
    Status = uefi_call_wrapper(ConOut->QueryMode, 4, ConOut, ConOut->Mode->Mode, &Columns, &Rows);
    if (EFI_ERROR(Status)) {
        Print(L"Error querying screen size.\n");
        return;
    }

    CHAR16 rotatingChar;
    UINTN defaultColor = EFI_LIGHTGRAY;  // Set a default color (light gray in this case)

    // List of all available colors
    UINTN colors[] = {
        EFI_BLACK, EFI_BLUE, EFI_GREEN, EFI_CYAN, EFI_RED, EFI_MAGENTA, 
        EFI_BROWN, EFI_LIGHTGRAY, EFI_DARKGRAY, EFI_LIGHTBLUE, EFI_LIGHTGREEN, 
        EFI_LIGHTCYAN, EFI_LIGHTRED, EFI_LIGHTMAGENTA, EFI_YELLOW, EFI_WHITE
    };

    UINTN totalColors = sizeof(colors) / sizeof(colors[0]);

    // Loop through all colors once
    for (UINTN colorIndex = 0; colorIndex < totalColors; colorIndex++) {
        UINTN color = colors[colorIndex];
        uefi_call_wrapper(ConOut->SetAttribute, 1, ConOut, color);

        // Display rotating characters across the entire screen
        for (UINTN i = 0; i < 4; i++) {
            rotatingChar = (i % 4 == 0) ? L'/' :
                           (i % 4 == 1) ? L'-' :
                           (i % 4 == 2) ? L'|' : L'\\';

            // Clear the screen before each iteration to avoid character overlap
            uefi_call_wrapper(ConOut->ClearScreen, 1, ConOut);

            for (UINTN row = 0; row < Rows; row++) {
                for (UINTN col = 0; col < Columns; col++) {
                    uefi_call_wrapper(ConOut->SetCursorPosition, 2, ConOut, col, row);
                    Print(L"%c", rotatingChar);
                }
            }

            // Add delay between rotations to make the effect visible
            delay(500000);  // Adjust the delay as needed
        }

        // Add delay between color changes to make the transition visible
        delay(500000);  // Adjust the delay as needed
    }

    // Reset to default color after the effect
    uefi_call_wrapper(ConOut->SetAttribute, 1, ConOut, defaultColor);
}

// Effect 2: Complex Blur
void complex_blur(EFI_SYSTEM_TABLE* SystemTable, CHAR16* message, UINTN originalCol, UINTN originalRow) {
    EFI_SIMPLE_TEXT_OUT_PROTOCOL* ConOut = SystemTable->ConOut;
    UINTN col, row;
    UINTN Columns, Rows;
    int blur_intensity = 6;
    EFI_STATUS Status;

    // Get the dimensions of the screen
    Status = uefi_call_wrapper(ConOut->QueryMode, 4, ConOut, ConOut->Mode->Mode, &Columns, &Rows);
    if (EFI_ERROR(Status)) {
        Print(L"Error querying screen size.\n");
        return;
    }

    for (int i = 0; i < blur_intensity; i++) {
        col = originalCol + (my_rand() % 50 - 25);
        row = originalRow + (my_rand() % 30 - 15);

        // Ensure coordinates are within screen boundaries
        if (col >= Columns) col = Columns - 1;
        if (row >= Rows) row = Rows - 1;

        UINTN color = random_color();
        uefi_call_wrapper(ConOut->SetAttribute, 1, ConOut, color);
        uefi_call_wrapper(ConOut->SetCursorPosition, 2, ConOut, col, row);
        Print(message);
    }

    // Reset to default color and position
    uefi_call_wrapper(ConOut->SetAttribute, 1, ConOut, EFI_WHITE | EFI_BACKGROUND_BLACK);
    uefi_call_wrapper(ConOut->SetCursorPosition, 2, ConOut, originalCol, originalRow);
    Print(message);
}

// Effect 3: Pixel Rain
void pixel_rain(EFI_SYSTEM_TABLE* SystemTable, UINTN Columns, UINTN Rows) {
    for (UINTN i = 0; i < 100; i++) {
        UINTN col = my_rand() % Columns;
        UINTN row = my_rand() % Rows;
        UINTN color = random_color();
        uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, color);
        uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, col, row);
        Print(L".");
        delay(500000);
    }
}

// Effect 4: Wave Animation
void wave_animation(EFI_SYSTEM_TABLE* SystemTable, CHAR16* message, UINTN originalCol, UINTN originalRow, UINTN amplitude) {
    for (UINTN t = 0; t < 100; t++) {
        for (UINTN i = 0; i < StrLen(message); i++) {
            UINTN col = originalCol + i;
            UINTN row = originalRow + (amplitude * Sin((t + i) % 360));
            UINTN color = random_color();
            uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, color);
            uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, col, row);
            Print(L"%c", message[i]);
        }
        delay(1000000);
    }
}

// Effect 5: Text Explosion
void text_explosion(EFI_SYSTEM_TABLE* SystemTable, CHAR16* message, UINTN originalCol, UINTN originalRow) {
    for (UINTN i = 0; i < 50; i++) {
        UINTN col = originalCol + (my_rand() % 50 - 25);
        UINTN row = originalRow + (my_rand() % 30 - 15);
        UINTN color = random_color();
        uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, color);
        uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, col, row);
        Print(message);
        delay(2000000);
    }
}

// Effect 6: Fading Text
void fading_text(EFI_SYSTEM_TABLE* SystemTable, CHAR16* message, UINTN originalCol, UINTN originalRow) {
    for (UINTN i = 0; i < 20; i++) {
        UINTN color = random_color();
        uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, color);
        uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, originalCol, originalRow);
        Print(message);
        delay(2000000);
        uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);
    }
}

// Effect 7: Grid Animation
void grid_animation(EFI_SYSTEM_TABLE* SystemTable, UINTN Columns, UINTN Rows) {
    for (UINTN i = 0; i < 50; i++) {
        for (UINTN row = 0; row < Rows; row += 2) {
            for (UINTN col = 0; col < Columns; col += 2) {
                UINTN color = random_color();
                uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, color);
                uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, col, row);
                Print(L"[]");
            }
        }
        delay(1000000);
    }
}

// Effect 8: Matrix-like Text Fall
void matrix_text_fall(EFI_SYSTEM_TABLE* SystemTable, UINTN Columns, UINTN Rows) {
    for (UINTN i = 0; i < 100; i++) {
        for (UINTN j = 0; j < Columns; j++) {
            UINTN row = my_rand() % Rows;
            UINTN col = my_rand() % Columns;
            UINTN color = random_color();
            uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, color);
            uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, col, row);
            CHAR16 symbol = (my_rand() % 2 == 0) ? L'1' : L'0';
            Print(L"%c", symbol);
        }
        delay(500000);
    }
}

// Effect 9: Color Shifting
void color_shifting(EFI_SYSTEM_TABLE* SystemTable) {
    for (UINTN i = 0; i < 100; i++) {
        UINTN color = random_color();
        uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, color);
        uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, 0, 0);
        Print(L"Color shifting effect");
        delay(2000000);
    }
}

// Effect 10: Flickering Text
void flickering_text(EFI_SYSTEM_TABLE* SystemTable, CHAR16* message, UINTN originalCol, UINTN originalRow) {
    for (UINTN i = 0; i < 50; i++) {
        UINTN color = random_color();
        uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, color);
        uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, originalCol, originalRow);
        Print(message);
        delay(1000000);
        uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);
        delay(1000000);
    }
}

// Effect 11: Flashing Borders
void flashing_borders(EFI_SYSTEM_TABLE* SystemTable, UINTN Columns, UINTN Rows) {
    for (UINTN i = 0; i < 50; i++) {
        UINTN color = random_color();
        uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, color);
        for (UINTN col = 0; col < Columns; col++) {
            uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, col, 0);
            Print(L"-");
            uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, col, Rows - 1);
            Print(L"-");
        }
        for (UINTN row = 0; row < Rows; row++) {
            uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, 0, row);
            Print(L"|");
            uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, Columns - 1, row);
            Print(L"|");
        }
        delay(500000);
        uefi_call_wrapper(SystemTable->ConOut->ClearScreen, 1, SystemTable->ConOut);
    }
}

// Effect 12: Random Patterns and Symbols
void random_patterns_symbols(EFI_SYSTEM_TABLE* SystemTable, UINTN Columns, UINTN Rows) {
    CHAR16 symbols[] = { L'@', L'#', L'$', L'%', L'&', L'*', L'+', L'=' };  // Array of random symbols
    UINTN num_symbols = sizeof(symbols) / sizeof(symbols[0]);

    for (UINTN i = 0; i < 100; i++) {
        for (UINTN j = 0; j < 50; j++) {
            UINTN col = my_rand() % Columns;
            UINTN row = my_rand() % Rows;
            UINTN color = random_color();
            CHAR16 symbol = symbols[my_rand() % num_symbols];

            uefi_call_wrapper(SystemTable->ConOut->SetAttribute, 1, SystemTable->ConOut, color);
            uefi_call_wrapper(SystemTable->ConOut->SetCursorPosition, 1, SystemTable->ConOut, col, row);
            Print(L"%c", symbol);
        }
        delay(500000);
    }
}

// Function to play a beep sound with a specific frequency and duration
void play_beep(UINTN frequency, UINTN duration) {
    UINTN divisor = PIT_FREQUENCY / frequency;

    // Set the frequency for the PC speaker
    outb(TIMER_COMMAND_PORT, 0xB6);
    outb(TIMER_DATA_PORT, divisor & 0xFF);
    outb(TIMER_DATA_PORT, (divisor >> 8) & 0xFF);

    // Turn on the speaker
    UINT8 temp = inb(SPEAKER_PORT);
    outb(SPEAKER_PORT, temp | 0x03);

    // Delay for the duration of the sound
    delay(duration);

    // Turn off the speaker
    temp = inb(SPEAKER_PORT);
    outb(SPEAKER_PORT, temp & 0xFC);
}

// Function to create visual and sound effects
void dramatic_sound_effects(EFI_SYSTEM_TABLE* SystemTable, UINTN Columns, UINTN Rows, UINTN effectDuration) {
    EFI_SIMPLE_TEXT_OUT_PROTOCOL* ConOut = SystemTable->ConOut;
    UINTN iterations = effectDuration / 1000000; // Convert microseconds to iterations

    for (UINTN i = 0; i < iterations; i++) {
        // Simulate sound with a flashing effect and PC speaker beep
        UINTN color = (i % 2 == 0) ? EFI_RED : EFI_WHITE;
        uefi_call_wrapper(ConOut->SetAttribute, 1, ConOut, color);
        uefi_call_wrapper(ConOut->ClearScreen, 1, ConOut);
        uefi_call_wrapper(ConOut->SetCursorPosition, 1, ConOut, Columns / 2 - 15, Rows / 2);
        Print(L"UTKUDORUK IS HERE!");

        // Play a beep sound with alternating frequencies
        play_beep((i % 2 == 0) ? 1000 : 2000, 50000); // Frequency alternates between 1kHz and 2kHz

        // Add patterns to simulate varying sound effects
        if (i % 5 == 0) {
            uefi_call_wrapper(ConOut->SetCursorPosition, 1, ConOut, Columns / 2 - 15, Rows / 2 + 1);
            Print(L"THERE IS NO ESCAPE!");
            play_beep(1500, 100000); // Play a beep at 1.5kHz
        }

        // Additional pattern for simulating sound intensity
        if (i % 10 == 0) {
            uefi_call_wrapper(ConOut->SetAttribute, 1, ConOut, EFI_GREEN);
            uefi_call_wrapper(ConOut->ClearScreen, 1, ConOut);
            uefi_call_wrapper(ConOut->SetCursorPosition, 1, ConOut, Columns / 2 - 15, Rows / 2 + 2);
            Print(L"!!!");
            play_beep(3000, 50000); // Play a higher-pitched beep at 3kHz
        }

        delay(500000); // Simulate duration
    }
}

// Updated Main EFI Entry Point
EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable) {
    InitializeLib(ImageHandle, SystemTable);

    // Get screen size (columns and rows)
    UINTN Columns, Rows;
    EFI_STATUS Status = uefi_call_wrapper(SystemTable->ConOut->QueryMode, 4, SystemTable->ConOut, SystemTable->ConOut->Mode->Mode, &Columns, &Rows);
    if (EFI_ERROR(Status)) {
        Print(L"Unable to query screen size\n");
        return Status;
    }

    // Effect Duration
    UINTN effectDuration = 10000000; // Duration for all effects (in microseconds)

    // Run visual effects with dramatic sound effect simulation
    rotating_characters_all_colors(SystemTable);
    dramatic_sound_effects(SystemTable, Columns, Rows, effectDuration);

    complex_blur(SystemTable, L"UTKUDORUK HERE!", 10, 10);
    dramatic_sound_effects(SystemTable, Columns, Rows, effectDuration);

    pixel_rain(SystemTable, Columns, Rows);
    dramatic_sound_effects(SystemTable, Columns, Rows, effectDuration);

    wave_animation(SystemTable, L"UTKUDORUK HERE!", 0, 10, 2);
    dramatic_sound_effects(SystemTable, Columns, Rows, effectDuration);

    text_explosion(SystemTable, L"UTKUDORUK HERE!", Columns / 2, Rows / 2);
    dramatic_sound_effects(SystemTable, Columns, Rows, effectDuration);

    fading_text(SystemTable, L"UTKUDORUK HERE!", 5, 5);
    dramatic_sound_effects(SystemTable, Columns, Rows, effectDuration);

    grid_animation(SystemTable, Columns, Rows);
    dramatic_sound_effects(SystemTable, Columns, Rows, effectDuration);

    matrix_text_fall(SystemTable, Columns, Rows);
    dramatic_sound_effects(SystemTable, Columns, Rows, effectDuration);

    color_shifting(SystemTable);
    dramatic_sound_effects(SystemTable, Columns, Rows, effectDuration);

    flickering_text(SystemTable, L"UTKUDORUK HERE!", 20, 10);
    dramatic_sound_effects(SystemTable, Columns, Rows, effectDuration);

    flashing_borders(SystemTable, Columns, Rows);
    dramatic_sound_effects(SystemTable, Columns, Rows, effectDuration);

    random_patterns_symbols(SystemTable, Columns, Rows);
    dramatic_sound_effects(SystemTable, Columns, Rows, effectDuration);

    return EFI_SUCCESS;
}
