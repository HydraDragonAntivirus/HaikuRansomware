#include "MainWindow.h"

#include <Application.h>
#include <Catalog.h>
#include <File.h>
#include <FindDirectory.h>
#include <LayoutBuilder.h>
#include <Menu.h>
#include <MenuBar.h>
#include <Path.h>
#include <View.h>
#include <cstdlib>
#include <iostream>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <chrono>     // For std::chrono
#include <set>

#undef B_TRANSLATION_CONTEXT
#define B_TRANSLATION_CONTEXT "Window"

static const uint32 kMsgStartMonitor = 'strt';
static const uint32 kMsgQuitApp = 'quit';

static const char* kSettingsFile = "Ransomware Detection Settings";

namespace fs = std::filesystem;

// List of known file extensions
std::vector<std::string> knownExtensions = {".pyd", ".elf", ".ps1", ".bas", ".bat", ".chm", ".cmd", ".com", ".cpl", ".dll", ".exe", ".msc", ".ocx", ".pcd", ".pif", ".reg", ".scr", ".sct", ".url", ".vbe", ".wsc", ".wsf", ".wsh", ".ct", ".t", ".input", ".war", ".jspx", ".tmp", ".dump", ".pwd", ".w", ".cfg", ".psd1", ".psm1", ".ps1xml", ".clixml", ".psc1", ".pssc", ".www", ".rdp", ".msi", ".dat", ".contact", ".settings", ".odt", ".jpg", ".mka", "shtml", ".mhtml", ".oqy", ".png", ".csv", ".py", ".sql", ".mdb", ".html", ".htm", ".xml", ".psd", ".pdf", ".xla", ".cub", ".dae", ".indd", ".cs", ".mp3", ".mp4", ".dwg", ".rar", ".mov", ".rtf", ".bmp", ".mkv", ".avi", ".apk", ".lnk", ".dib", ".dic", ".dif", ".divx", ".iso", ".7zip", ".ace", ".arj", ".bz2", ".cab", ".gzip", ".lzh", ".jpeg", ".xz", ".mpeg", ".torrent", ".mpg", ".core", ".pdb", ".ico", ".pas", ".db", ".wmv", ".swf", ".cer", ".bak", ".backup", ".accdb", ".bay", ".p7c", ".exif", ".vss", ".raw", ".m4a", ".wma", ".flv", ".sie", ".sum", ".ibank", ".wallet", ".css", ".js", ".rb", ".xlsm", ".xlsb", ".7z", ".cpp", ".java", ".jpe", ".ini", ".blob", ".wps", ".wav", ".3gp", ".webm", ".m4v", ".amv", ".m4p", ".svg", ".ods", ".bk", ".vdi", ".vmdk", ".accde", ".json", ".gif", ".gz", ".m1v", ".sln", ".pst", ".obj", ".xlam", ".djvu", ".inc", ".cvs", ".dbf", ".tbi", ".wpd", ".dot", ".dotx", ".xltx", ".pptm", ".potx", ".potm", ".xlw", ".xps", ".xsd", ".xsf", ".xsl", ".kmz", ".accdr", ".stm", ".accdt", ".ppam", ".pps", ".ppsm", ".1cd", ".3ds", ".3fr", ".3g2", ".accda", ".accdc", ".accdw", ".adp", ".ai", ".ai3", ".ai4", ".ai5", ".ai6", ".ai7", ".ai8", ".arw", ".ascx", ".asm", ".asmx", ".avs", ".bin", ".cfm", ".dbx", ".dcm", ".dcr", ".pict", ".rgbe", ".dwt", ".f4v", ".exr", ".kwm", ".max", ".mda", ".mde", ".mdf", ".mdw", ".mht", ".mpv", ".msg", ".myi", ".nef", ".odc", ".geo", ".swift", ".odm", ".odp", ".oft", ".orf", ".pfx", ".p12", ".pls", ".safe", ".tab", ".vbs", ".xlk", ".xlm", ".xlt", ".xltm", ".svgz", ".slk", ".dmg", ".ps", ".psb", ".tif", ".rss", ".key", ".vob", ".epsp", ".dc3", ".iff", ".onepkg", ".onetoc2", ".opt", ".p7b", ".pam", ".r3d", ".pkg", ".yml", ".old", ".thmx", ".keytab", ".h", ".php", ".c", ".zip", ".log", ".log1", ".log2", ".tm", ".blf", ".uic", ".widget-plugin", ".regtrans-ms", ".efi", ".rule", ".rules", ".yar", ".yara", ".yrc", ".inf", ".ini", ".ndb", ".cvd", ".cld", ".ign2", ".dmp", ".conf.config", ".pyc", ".386", ".3gp2", ".3gpp", ".3mf", ".a", ".a2s", ".aac", ".ac3", ".accessor", ".accountpicture-ms", ".adt", ".adts", ".aif", ".aifc", ".aiff", ".androidproj", ".ani", ".ans", ".appcontent-ms", ".application", ".appref-ms", ".aps", ".arc", ".ari", ".art", ".asa", ".asax", ".asc", ".asf", ".ashx", ".asp", ".aspx", ".asx", ".au", ".avci", ".avcs", ".avif", ".avifs", ".bcp", ".bkf", ".blg", ".bsc", ".camp", ".cap", ".cat", ".cc", ".cda", ".cdmp", ".cdx", ".cdxml", ".cgm", ".chk", ".cjs", ".cls", ".cod", ".coffee", ".compositefont", ".config", ".coverage", ".cppm", ".cr2", ".cr3", ".crl", ".crt", ".crw", ".csa", ".csh", ".cshader", ".cshtml", ".csproj", ".cts", ".cur", ".cxx", ".datasource", ".dbg", ".dbs", ".dcs", ".dct", ".dctx", ".dctxc", ".dds", ".def", ".der", ".desklink", ".deskthemepack", ".devicemanifest-ms", ".devicemetadata-ms", ".diagcab", ".diagcfg", ".diagpkg", ".diagsession", ".disco", ".diz", ".dl_", ".dng", ".doc", ".docx", ".dos", ".drf", ".drv", ".dsgl", ".dsh", ".dshader", ".dsn", ".dsp", ".dsw", ".dtcp-ip", ".dtd", ".dvr-ms", ".ec3", ".edmx", ".eip", ".emf", ".eml", ".eps", ".epub", ".erf", ".etl", ".etp", ".evt", ".evtx", ".exp", ".ext", ".ex_", ".eyb", ".faq", ".fff", ".fif", ".filters", ".fky", ".flac", ".fnd", ".fnt", ".fon", ".fx", ".generictest", ".ghi", ".gitattributes", ".gitignore", ".gitmodules", ".gmmp", ".group", ".grp", ".gsh", ".gshader", ".hdd", ".hdp", ".heic", ".heics", ".heif", ".heifs", ".hh", ".hhc", ".hif", ".hlp", ".hlsl", ".hlsli", ".hpp", ".hqx", ".hsh", ".hshader", ".hta", ".htc", ".htt", ".htw", ".htx", ".hxx", ".i", ".ibq", ".icc", ".icl", ".icm", ".ics", ".idb", ".idl", ".idq", ".igp", ".iiq", ".ilk", ".imc", ".imesx", ".img", ".inl", ".inv", ".inx", ".in_", ".ipp", ".itrace", ".ivf", ".ixx", ".jav", ".jbf", ".jfif", ".job", ".jod", ".jse", ".jsonld", ".jsproj", ".jsx", ".jxr", ".k25", ".kci", ".kdc", ".label", ".latex", ".less", ".lgn", ".lib", ".library-ms", ".lic", ".local", ".lpcm", ".lst", ".m14", ".m2t", ".m2ts", ".m2v", ".m3u", ".m4b", ".mak", ".man", ".manifest", ".map", ".mapimail", ".master", ".mef", ".mfcribbon-ms", ".mid", ".midi", ".mjs", ".mk", ".mk3d", ".mlc", ".mmf", ".mod", ".mos", ".movie", ".mp2", ".mp2v", ".mp4v", ".mpa", ".mpe", ".mpv2", ".mrw", ".ms-windows-store-license", ".msepub", ".msm", ".msp", ".msrcincident", ".msstyles", ".msu", ".mts", ".mtx", ".mv", ".mydocs", ".natvis", ".ncb", ".netperf", ".nettrace", ".nfo", ".nls", ".nrw", ".nvr", ".nvram", ".oc_", ".odh", ".odl", ".oga", ".ogg", ".ogm", ".ogv", ".ogx", ".opus", ".orderedtest", ".ori", ".osdx", ".otf", ".ova", ".ovf", ".p10", ".p7m", ".p7r", ".p7s", ".pal", ".partial", ".pbk", ".pch", ".pcp", ".pds", ".pef", ".perfmoncfg", ".pfm", ".php3", ".pic", ".pkgdef", ".pkgundef", ".pko", ".pl", ".plg", ".pma", ".pmc", ".pml", ".pmr", ".pnf", ".pot", ".ppkg", ".ppt", ".prc", ".prf", ".printerexport", ".props", ".psh", ".pshader", ".ptx", ".publishproj", ".pubxml", ".pxn", ".pyo", ".pyw", ".pyz", ".pyzw", ".qds", ".raf", ".rat", ".razor", ".rc", ".rc2", ".rct", ".res", ".resmoncfg", ".resw", ".resx", ".rgs", ".rle", ".rll", ".rmi", ".rpc", ".rsp", ".rul", ".ruleset", ".rw2", ".rwl", ".s", ".sbr", ".sc2", ".scc", ".scd", ".scf", ".sch", ".scp", ".scss", ".sdl", ".search-ms", ".searchconnector-ms", ".sed", ".settingcontent-ms", ".sfcache", ".sh", ".shproj", ".shtm", ".shtml", ".sit", ".sitemap", ".skin", ".slnf", ".snd", ".snippet", ".snk", ".sol", ".sor", ".spc", ".sr2", ".srf", ".srw", ".sr_", ".sst", ".stvproj", ".suo", ".svc", ".svclog", ".sym", ".symlink", ".sys", ".sy_", ".tar", ".targets", ".tdl", ".testrunconfig", ".testsettings", ".text", ".tgz", ".theme", ".themepack", ".tiff", ".tlb", ".tlh", ".tli", ".tod", ".tpsr", ".trg", ".trx", ".ts", ".tsp", ".tsv", ".tsx", ".tt", ".ttc", ".ttf", ".tts", ".tvc", ".tvlink", ".tvs", ".txt", ".udf", ".udl", ".udt", ".uitest", ".user", ".usr", ".uvu", ".vb", ".vbhtml", ".vbox", ".vbox-extpack", ".vbproj", ".vbx", ".vcf", ".vcproj", ".vcxitems", ".vcxproj", ".vhd", ".vhdpmem", ".vhdx", ".viw", ".vmac", ".vmba", ".vmpl", ".vmsd", ".vmsn", ".vmss", ".vmt", ".vmtm", ".vmx", ".vmxf", ".vsct", ".vsglog", ".vsh", ".vshader", ".vsix", ".vsixlangpack", ".vsixmanifest", ".vsmdi", ".vsp", ".vsprops", ".vsps", ".vspscc", ".vsscc", ".vssettings", ".vssscc", ".vstemplate", ".vsz", ".vxd", ".wab", ".wax", ".wbcat", ".wcx", ".wdp", ".weba", ".webp", ".webpnp", ".website", ".wll", ".wlt", ".wm", ".wmd", ".wmdb", ".wmf", ".wmp", ".wms", ".wmx", ".wmz", ".wpa", ".wpapk", ".wpl", ".wri", ".wsdl", ".wsz", ".wtv", ".wtx", ".wvx", ".x", ".x3f", ".xaml", ".xbap", ".xdr", ".xht", ".xhtml", ".xix", ".xlb", ".xlc", ".xls", ".xproj", ".xrm-ms", ".xsc", ".xslt", ".xss", ".z", ".z96", ".zfsendtotarget", ".zoo", "._bsln140", "._bsln150", "._sln", "._sln100", "._sln110", "._sln120", "._sln140", "._sln150", "._sln160", "._sln170", "._sln60", "._sln70", "._sln71", "._sln80", "._sln90", "._vbxsln100", "._vbxsln110", "._vbxsln80", "._vbxsln90", "._vcppxsln100", "._vcppxsln110", "._vcppxsln80", "._vcppxsln90", "._vcsxsln100", "._vcsxsln110", "._vcsxsln80", "._vcsxsln90", "._vjsxsln80", "._vw8xsln110", "._vwdxsln100", "._vwdxsln110", "._vwdxsln120", "._vwdxsln140", "._vwdxsln150", "._vwdxsln80", "._vwdxsln90", "._vwinxsln120", "._vwinxsln140", "._vwinxsln150", "._wdxsln110", "._wdxsln120", "._wdxsln140", "._wdxsln150", ".all", ".amr", ".appinstaller", ".appx", ".appxbundle", ".c5e2524a-ea46-4f67-841f-6a9465d9d515", ".conf", ".daq", ".dpl", ".fbx", ".fd", ".fh", ".fud", ".glb", ".gltf", ".ids", ".iss", ".list", ".m3u8", ".m4r", ".md", ".mdc", ".mpg4", ".ms-lockscreencomponent-primary", ".msix", ".msixbundle", ".nupkg", ".one", ".oxps", ".ply", ".reputation", ".rwz", ".sample", ".sig", ".solitairetheme8", ".stl", ".thumb", ".winget", ".winmd", ".wsb", ".xvid", ".yaml", ".zpl", ".boe", ".cwp", ".jar", ".jnlp", ".jxl", ".lfm", ".lpi", ".lpk", ".lpr", ".oetpl", ".ovpn", ".pp", ".soe", ".tbz2", ".txz", ".tzst", ".wdq", ".zst", ".cnf", ".d", ".data", ".f4a", ".fluid", ".hdmp", ".kdmp", ".lastbuildstate", ".loop", ".mdmp", ".mxf", ".ndmp", ".note", ".opdownload", ".pyx", ".qt", ".recipe", ".rm", ".rmv", ".rmvb", ".run", ".tlog", ".whiteboard", ".yuv",".0", ".0nv", ".0rv", ".1", ".10", ".10-config", ".100", ".102", ".10nv", ".10rv", ".12", ".13", ".14", ".15", ".16", ".164", ".17", ".18", ".199", ".1m", ".2", ".20", ".21", ".25", ".2538)", ".25nv", ".25rv", ".26", ".27", ".28", ".29", ".3", ".30", ".32", ".34", ".39", ".3am", ".3ossl", ".4", ".5", ".58", ".59", ".6", ".60", ".62", ".7", ".72", ".74", ".8", ".9", ".97", ".ATAPI", ".Be-MailDraft", ".CDDL", ".CHANGES", ".DiskT@2", ".EAP", ".Eterm", ".Haiku-desklink", ".LESSER", ".LGPL", ".SKIP", ".WORM", ".about", ".accelerant", ".addon-host", ".addonhost", ".afm", ".am", ".ambdec", ".app", ".applescript", ".audio", ".autoraise", ".awk", ".base", ".be-bookmark", ".be-directory", ".be-elfexecutable", ".be-input_server", ".be-kmap", ".be-mail", ".be-maildraft", ".be-pepl", ".be-post", ".be-pref", ".be-prnt", ".be-psrv", ".be-query", ".be-querytemplate", ".be-root", ".be-symlink", ".be-trak", ".be-tskb", ".be-volume", ".be-work", ".beshare", ".bfd", ".bios_ia32", ".build", ".ca", ".cache", ".cache-8", ".catalog", ".categories", ".cdplus", ".cdrw", ".cdtext", ".ch", ".chart", ".chart-template", ".cidToUnicode", ".clone", ".cmake", ".compression", ".copy", ".cpg", ".ctypes", ".database", ".db-journal", ".db-shm", ".db-wal", ".dd", ".de", ".default", ".deps", ".desktop", ".devhelp2", ".dict", ".dist", ".doi", ".e2x", ".el", ".eltorito", ".enc", ".example", ".falkon", ".ffpreset", ".file", ".finger", ".fish", ".formula", ".formula-template", ".fortune", ".fr", ".frag", ".ftp", ".genio", ".gep", ".gir", ".git", ".gnome", ".gopher", ".gr", ".graft_dirs", ".graphics", ".graphics-template", ".guess", ".gutenprint", ".haiku-about", ".haiku-activitymonitor", ".haiku-app_server", ".haiku-appearance", ".haiku-aviftranslator", ".haiku-backgrounds", ".haiku-bfsaddon", ".haiku-bluetooth_server", ".haiku-bluetoothprefs", ".haiku-bmptranslator", ".haiku-bootmanager", ".haiku-butterflyscreensaver", ".haiku-cddb_lookup", ".haiku-charactermap", ".haiku-chartdemo", ".haiku-checkitout", ".haiku-clock", ".haiku-cmd-dstconfig", ".haiku-codycam", ".haiku-core-file", ".haiku-datatranslations", ".haiku-debug_server", ".haiku-debugger", ".haiku-debugnowscreensaver", ".haiku-deskbarpreferences", ".haiku-deskcalc", ".haiku-desklink", ".haiku-devices", ".haiku-diskprobe", ".haiku-diskusage", ".haiku-dns-resolver-server", ".haiku-dnsclientservice", ".haiku-drivesetup", ".haiku-ehci", ".haiku-expander", ".haiku-fataddon", ".haiku-filetypes", ".haiku-firstbootprompt", ".haiku-flurryscreensaver", ".haiku-fontdemo", ".haiku-fortune", ".haiku-ftpservice", ".haiku-giftranslator", ".haiku-glifescreensaver", ".haiku-glinfo", ".haiku-glteapot", ".haiku-gptdiskaddon", ".haiku-gravityscreensaver", ".haiku-haiku3d", ".haiku-haikudepot", ".haiku-hostname", ".haiku-hviftranslator", ".haiku-icnstranslator", ".haiku-icon", ".haiku-icon_o_matic", ".haiku-iconsscreensaver", ".haiku-icotranslator", ".haiku-ifsscreensaver", ".haiku-imap", ".haiku-input", ".haiku-installer", ".haiku-inteldiskaddon", ".haiku-ipv4interface", ".haiku-ipv6interface", ".haiku-jpeg2000translator", ".haiku-jpegtranslator", ".haiku-keyboardinputserverdevice", ".haiku-keymap", ".haiku-keymap-cli", ".haiku-keystore-cli", ".haiku-keystore_server", ".haiku-launch_daemon", ".haiku-launchbox", ".haiku-leavesscreensaver", ".haiku-libbe", ".haiku-libmail", ".haiku-libmedia", ".haiku-libpackage", ".haiku-libtextencoding", ".haiku-libtracker", ".haiku-locale", ".haiku-magnify", ".haiku-mail", ".haiku-mail2mbox", ".haiku-mail_utils-mail", ".haiku-mandelbrot", ".haiku-markas", ".haiku-markasread", ".haiku-matchheader", ".haiku-mbox2mail", ".haiku-media", ".haiku-mediaconverter", ".haiku-mediaplayer", ".haiku-messagescreensaver", ".haiku-midi_server", ".haiku-midiplayer", ".haiku-mount_server", ".haiku-mountvolume", ".haiku-nebulascreensaver", ".haiku-net_server", ".haiku-network", ".haiku-networkstatus", ".haiku-newmailnotification", ".haiku-nfs4_idmapper-server", ".haiku-notification_server", ".haiku-notifications", ".haiku-notify", ".haiku-ntfsdiskaddon", ".haiku-ohci", ".haiku-openterminal", ".haiku-overlayimage", ".haiku-package", ".haiku-package_daemon", ".haiku-packageinstaller", ".haiku-pairs", ".haiku-pcxtranslator", ".haiku-playground", ".haiku-playlist", ".haiku-pngtranslator", ".haiku-poorman", ".haiku-pop3", ".haiku-powermanagement", ".haiku-powerstatus", ".haiku-ppmtranslator", ".haiku-print-addon-server", ".haiku-processcontroller", ".haiku-psdtranslator", ".haiku-pulse", ".haiku-rawtranslator", ".haiku-registrar", ".haiku-remotedesktop", ".haiku-repositories", ".haiku-rtftranslator", ".haiku-screen", ".haiku-screenmode", ".haiku-screensaver", ".haiku-screenshot", ".haiku-screenshot-cli", ".haiku-sgitranslator", ".haiku-shelfscreensaver", ".haiku-shortcuts", ".haiku-showimage", ".haiku-smtp", ".haiku-softwareupdater", ".haiku-soundrecorder", ".haiku-sounds", ".haiku-spamfilter", ".haiku-spiderscreensaver", ".haiku-sshservice", ".haiku-stxttranslator", ".haiku-stylededit", ".haiku-sudoku", ".haiku-systemlogger", ".haiku-telnetservice", ".haiku-terminal", ".haiku-tgatranslator", ".haiku-tifftranslator", ".haiku-time", ".haiku-trackerpreferences", ".haiku-uhci", ".haiku-urlwrapper", ".haiku-usb", ".haiku-virtual-directory", ".haiku-virtualmemory", ".haiku-webpositive", ".haiku-webptranslator", ".haiku-wonderbrushtranslator", ".haiku-xhci", ".hekkel-pe-group", ".hfs_boot", ".hfs_magic", ".hide", ".hpkg", ".http", ".https", ".icon", ".icq", ".idx", ".image", ".image-template", ".in", ".info", ".info-1", ".info-2", ".info-3", ".info-4", ".info-5", ".info-6", ".info-7", ".ink-vision", ".interface", ".introspection", ".its", ".joliet", ".jsp", ".kapix-koder", ".konsole", ".konsole-256color", ".l", ".ldb", ".linux", ".linux-m1", ".linux-m1b", ".linux-m2", ".linux-s", ".lips3-compatible", ".lips4-compatible", ".ll", ".lo", ".loc", ".m4", ".m4f", ".macosx", ".mailto", ".malinen-wpa_supplicant", ".mcp", ".media-server", ".media_addon", ".mesh", ".metainfo", ".mgc", ".mhr", ".mimeset", ".minitel1", ".minitel1-nb", ".minitel12-80", ".minitel1b", ".minitel1b-80", ".minitel1b-nb", ".minitel2-80", ".mkhybrid", ".mlterm", ".mlterm-256color", ".mms", ".mo", ".module", ".modulemap", ".mp-xpdf", ".mpegurl", ".mrxvt", ".ms-excel", ".ms-powerpoint", ".ms-works", ".ms-xpsdocument", ".msn", ".multi", ".myname-myapp", ".nameToUnicode", ".nanorc", ".net", ".news", ".nexus-keymapswitcher", ".nfs", ".nodeset", ".o", ".old1", ".opentargetfolder", ".pack", ".pak", ".parallel", ".paranoia", ".patchbay", ".pc", ".pcl5-compatible", ".pcl6-compatible", ".pdfwriter", ".pe", ".pem", ".pfa", ".pfb", ".photoshop", ".pickle", ".picture", ".pm", ".pnvm", ".po", ".pod", ".prep_boot", ".presentation", ".presentation-template", ".preview", ".printer", ".printer-spool", ".productive-document", ".properties", ".prs", ".ps-compatible", ".pub", ".putty", ".putty-256color", ".putty-m1", ".putty-m1b", ".putty-m2", ".pxe_ia32", ".qm", ".qml", ".qmlc", ".qmltypes", ".qnotify-gate", ".qph", ".qtconfigurator", ".qtsystraymanager", ".query", ".rdef", ".renamecategories", ".resourcedef", ".rev", ".rez", ".rn-realmedia-vbr", ".rnc", ".rootinfo", ".route", ".rscsi", ".rst", ".rsync", ".rtp", ".rtsp", ".ru", ".rxvt", ".screenblanker", ".serialconnect", ".session", ".setmime", ".sf2", ".sftp", ".so", ".solaris-x86-ATAPI-DMA", ".solaris-x86-ata-DMA", ".sony", ".sort", ".spam_probability_database", ".spamdbm", ".sparcboot", ".spec", ".spreadsheet", ".spreadsheet-template", ".src", ".ssh", ".state", ".sub", ".summary", ".sun-lofi", ".sunx86boot", ".supp", ".svn", ".svn+ssh", ".sys-old", ".sysk", ".tcc", ".tcl", ".telnet", ".teraterm", ".tex", ".text-master", ".text-template", ".text-web", ".textsearch", ".thumbnailer", ".tmpl", ".typelib", ".ucode", ".unicodeMap", ".vapi", ".verify", ".vert", ".volmgt", ".vte", ".vte-256color", ".wbi", ".whl", ".xa", ".xbn", ".xc", ".xce", ".xd", ".xdc", ".xdce", ".xde", ".xdw", ".xdwe", ".xe", ".xn", ".xr", ".xs", ".xsce", ".xse", ".xsw", ".xswe", ".xterm-256color", ".xterm-new", ".xterm-r6", ".xterm-xfree86", ".xu", ".xw", ".xwe", ".y", ".yy", ".zip-o-matic"};

MainWindow::MainWindow()
    : BWindow(BRect(100, 100, 500, 400), B_TRANSLATE("Ransomware Detection"), B_TITLED_WINDOW,
              B_ASYNCHRONOUS_CONTROLS | B_QUIT_ON_WINDOW_CLOSE)
{
    BMenuBar* menuBar = _BuildMenu();

    BLayoutBuilder::Group<>(this, B_VERTICAL, 0)
        .Add(menuBar)
        .AddGlue()
        .End();

    BMessenger messenger(this);

    BMessage settings;
    _LoadSettings(settings);

    BRect frame;
    if (settings.FindRect("main_window_rect", &frame) == B_OK) {
        MoveTo(frame.LeftTop());
        ResizeTo(frame.Width(), Bounds().Height());
    }
    MoveOnScreen();
}

MainWindow::~MainWindow()
{
    _SaveSettings();
}

void MainWindow::MessageReceived(BMessage* message)
{
    switch (message->what) {
    case kMsgStartMonitor:
        StartMonitoring();
        break;

    case kMsgQuitApp:
        PostMessage(B_QUIT_REQUESTED);
        break;

    default:
        BWindow::MessageReceived(message);
        break;
    }
}

BMenuBar* MainWindow::_BuildMenu()
{
    BMenuBar* menuBar = new BMenuBar("menubar");
    BMenu* menu;
    BMenuItem* item;

    // 'Monitor' Menu
    menu = new BMenu(B_TRANSLATE("Monitor"));

    item = new BMenuItem(B_TRANSLATE("Start Monitoring"), new BMessage(kMsgStartMonitor), 'S');
    menu->AddItem(item);

    item = new BMenuItem(B_TRANSLATE("Quit"), new BMessage(kMsgQuitApp), 'Q');
    menu->AddItem(item);

    menuBar->AddItem(menu);

    // 'Help' Menu
    menu = new BMenu(B_TRANSLATE("Help"));
    item = new BMenuItem(B_TRANSLATE("About" B_UTF8_ELLIPSIS), new BMessage(B_ABOUT_REQUESTED));
    item->SetTarget(be_app);
    menu->AddItem(item);

    menuBar->AddItem(menu);

    return menuBar;
}

void MainWindow::StartMonitoring()
{
    printf("Monitoring started\n");
    MonitorDesktop();
}

void MainWindow::CheckFilesInDirectory(const std::string& directory, std::set<std::string>& processedFiles)
{
    // Monitor files in the specified directory
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_directory()) {
            // Recursively check subdirectories
            CheckFilesInDirectory(entry.path().string(), processedFiles);
        }
        else if (entry.is_regular_file()) {
            std::string filename = entry.path().filename().string();
            std::string extension = entry.path().extension().string();
            printf("Found file: %s with extension: %s\n", filename.c_str(), extension.c_str());

            // Check if this file has already been processed
            if (processedFiles.find(filename) != processedFiles.end()) {
                continue; // Skip if already processed
            }

            processedFiles.insert(filename); // Mark this file as processed

            if (filename.find('.') != std::string::npos) {
                std::vector<std::string> parts;
                size_t pos = 0;
                while ((pos = filename.find('.')) != std::string::npos) {
                    parts.push_back(filename.substr(0, pos));
                    filename.erase(0, pos + 1);
                }
                parts.push_back(filename);

                // Check the first extension
                if (std::find(knownExtensions.begin(), knownExtensions.end(), parts[0]) != knownExtensions.end()) {
                    // Analyze the last extension
                    if (std::find(knownExtensions.begin(), knownExtensions.end(), parts.back()) == knownExtensions.end()) {
                        // Unknown extension found, send shutdown command
                        printf("Unknown extension found: %s, shutting down...\n", parts.back().c_str());
                        system("shutdown -q");
                    }
                }
            }
        }
    }
}

// Update MonitorDesktop to initialize processedFiles set
void MainWindow::MonitorDesktop()
{
    // Get the Desktop path
    BPath desktopPath;
    if (find_directory(B_DESKTOP_DIRECTORY, &desktopPath) != B_OK) {
        printf("Error finding desktop directory\n");
        return;
    }

    std::string desktopDir = desktopPath.Path();

    // Set to keep track of processed file names
    std::set<std::string> processedFiles;

    // Continuously monitor the Desktop directory and its subdirectories
    while (true) {
        // Call the recursive function to check files
        CheckFilesInDirectory(desktopDir, processedFiles);
    }
}

status_t MainWindow::_LoadSettings(BMessage& settings)
{
    BPath path;
    status_t status;
    status = find_directory(B_USER_SETTINGS_DIRECTORY, &path);
    if (status != B_OK)
        return status;

    status = path.Append(kSettingsFile);
    if (status != B_OK)
        return status;

    BFile file;
    status = file.SetTo(path.Path(), B_READ_ONLY);
    if (status != B_OK)
        return status;

    return settings.Unflatten(&file);
}

status_t MainWindow::_SaveSettings()
{
    BPath path;
    status_t status = find_directory(B_USER_SETTINGS_DIRECTORY, &path);
    if (status != B_OK)
        return status;

    status = path.Append(kSettingsFile);
    if (status != B_OK)
        return status;

    BFile file;
    status = file.SetTo(path.Path(), B_WRITE_ONLY | B_CREATE_FILE | B_ERASE_FILE);
    if (status != B_OK)
        return status;

    BMessage settings;
    status = settings.AddRect("main_window_rect", Frame());

    if (status == B_OK)
        status = settings.Flatten(&file);

    return status;
}
