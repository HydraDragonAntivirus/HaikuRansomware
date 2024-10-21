#ifndef MAINWINDOW_H
#define MAINWINDOW_H


#include <FilePanel.h>
#include <MenuBar.h>
#include <MenuItem.h>
#include <Window.h>
#include <string>

class MainWindow : public BWindow
{
public:
							MainWindow();
	virtual					~MainWindow();

	virtual void			MessageReceived(BMessage* msg);

    void StartMonitoring();
    void MonitorDesktop();
	void CheckFilesInDirectory(const std::string& directory);

private:
			BMenuBar*		_BuildMenu();

			status_t		_LoadSettings(BMessage& settings);
			status_t		_SaveSettings();

			BMenuItem*		fSaveMenuItem;
			BFilePanel*		fOpenPanel;
			BFilePanel*		fSavePanel;
};

#endif
