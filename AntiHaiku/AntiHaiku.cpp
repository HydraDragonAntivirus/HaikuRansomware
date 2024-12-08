#include <Application.h>
#include <Window.h>
#include <View.h>
#include <Alert.h>
#include <Screen.h>
#include <MessageRunner.h>
#include <Bitmap.h>
#include <Region.h>
#include <stdlib.h> // For random number generation
#include <time.h>   // For seeding the random number generator

// Custom View to create flashing, distortion, and chaotic effects
class AntiHaikuView : public BView {
public:
    AntiHaikuView(BRect frame)
        : BView(frame, "AntiHaikuView", B_FOLLOW_ALL, B_WILL_DRAW) {
        SetViewColor(B_TRANSPARENT_COLOR);
        effectBitmap = nullptr;
        SetDrawingMode(B_OP_ALPHA);
    }

    ~AntiHaikuView() {
        delete effectBitmap;
    }

    void Draw(BRect updateRect) override {
        // Randomly apply one of the chaotic effects
        int effect = rand() % 6;

        switch (effect) {
        case 0:
            FlashScreen();
            break;
        case 1:
            BlurScreen();
            break;
        case 2:
            RandomShapes();
            break;
        case 3:
            InvertColors();
            break;
        case 4:
            ShrinkScreen();
            break;
        case 5:
            ReverseScreen();
            break;
        }
    }

private:
    BBitmap *effectBitmap;

    void FlashScreen() {
        // Random background color
        SetHighColor(rand() % 256, rand() % 256, rand() % 256);
        FillRect(Bounds());
    }

    void BlurScreen() {
        if (!effectBitmap) {
            effectBitmap = new BBitmap(Bounds(), B_RGBA32, true);
        }

        if (effectBitmap->Lock()) {
            BView *offscreenView = new BView(effectBitmap->Bounds(), "offscreenView", B_FOLLOW_ALL, B_WILL_DRAW);
            effectBitmap->AddChild(offscreenView);

            offscreenView->SetHighColor(200, 200, 200, 128); // Semi-transparent white
            offscreenView->FillRect(offscreenView->Bounds());
            offscreenView->Sync();
            effectBitmap->RemoveChild(offscreenView);
            delete offscreenView;

            DrawBitmap(effectBitmap);
            effectBitmap->Unlock();
        }
    }

    void RandomShapes() {
        for (int i = 0; i < 10; i++) {
            SetHighColor(rand() % 256, rand() % 256, rand() % 256, 128); // Semi-transparent
            BRect randomRect(rand() % (int)Bounds().Width(), rand() % (int)Bounds().Height(),
                             rand() % (int)Bounds().Width(), rand() % (int)Bounds().Height());
            FillEllipse(randomRect);
        }
    }

    void InvertColors() {
        SetHighColor(255, 255, 255);
        FillRect(Bounds());

        SetHighColor(0, 0, 0);
        StrokeLine(Bounds().LeftTop(), Bounds().RightBottom());
        StrokeLine(Bounds().LeftBottom(), Bounds().RightTop());
    }

    void ShrinkScreen() {
        BRect bounds = Bounds();
        float shrinkFactor = 0.8; // Shrink by 20%
        BRect shrunkRect(bounds.Width() * (1 - shrinkFactor) / 2,
                         bounds.Height() * (1 - shrinkFactor) / 2,
                         bounds.Width() * (1 + shrinkFactor) / 2,
                         bounds.Height() * (1 + shrinkFactor) / 2);

        SetHighColor(0, 0, 0); // Black background
        FillRect(bounds);
        SetHighColor(255, 255, 255); // White frame
        StrokeRect(shrunkRect);
    }

    void ReverseScreen() {
        SetHighColor(0, 0, 0); // Black background
        FillRect(Bounds());
        SetHighColor(rand() % 256, rand() % 256, rand() % 256);

        // Draw a mirror image effect by flipping coordinates
        for (int y = 0; y < Bounds().Height(); y++) {
            for (int x = 0; x < Bounds().Width() / 2; x++) {
                StrokeLine(BPoint(x, y), BPoint(Bounds().Width() - x - 1, y));
            }
        }
    }
};

// Main Application Window
class AntiHaikuWindow : public BWindow {
public:
    AntiHaikuWindow()
        : BWindow(BScreen().Frame(), "AntiHaiku Effects",
                  B_NO_BORDER_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
                  B_NOT_ZOOMABLE | B_NOT_RESIZABLE | B_AVOID_FRONT) {
        srand(time(nullptr)); // Seed random number generator
        view = new AntiHaikuView(Bounds());
        AddChild(view);

        // Timer-based updates for effects
        runner = new BMessageRunner(this, new BMessage('tick'), 100000); // Every 100ms
    }

    ~AntiHaikuWindow() override {
        delete runner;
    }

    bool QuitRequested() override {
        be_app->PostMessage(B_QUIT_REQUESTED);
        return true;
    }

    void MessageReceived(BMessage *message) override {
        if (message->what == 'tick' && view) {
            view->Invalidate(); // Trigger a redraw for effects
        } else {
            BWindow::MessageReceived(message);
        }
    }

private:
    AntiHaikuView *view;
    BMessageRunner *runner;
};

// Main Application
class AntiHaikuApp : public BApplication {
public:
    AntiHaikuApp()
        : BApplication("application/x-vnd.Haiku-AntiHaikuEffects") {}

    void ReadyToRun() override {
        ShowHealthWarning();

        AntiHaikuWindow *window = new AntiHaikuWindow();
        window->Show();
    }

private:
    void ShowHealthWarning() {
        BAlert *alert = new BAlert("Health Warning",
                                   "WARNING: This program produces rapid flashing lights and visual disturbances that may trigger seizures in individuals with photosensitive epilepsy. Press Escape to exit at any time.",
                                   "Exit", "Continue", nullptr, B_WIDTH_AS_USUAL, B_WARNING_ALERT);

        int32 response = alert->Go();
        if (response == 0) {
            // User chose to exit
            be_app->PostMessage(B_QUIT_REQUESTED);
        }
    }
};

int main() {
    AntiHaikuApp app;
    app.Run();
    return 0;
}
