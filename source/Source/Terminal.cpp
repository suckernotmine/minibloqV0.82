#include "Terminal.h"

#include <algorithm>
#include "hid.h"

#include <wx/msgdlg.h> //##Debug, se va en release. Agregar compilación condicional.


//##When add the header, mention that here is code From Alan Kharsansky too.


/////////////////////////////////////////////////////////////////////////////
//
// EmoticonScreen
//
/////////////////////////////////////////////////////////////////////////////
void EmoticonScreen::setEmoticonStr(const wxString& value)
{
    //##Unhardcode these comparisons:
    if (value == wxString(":)"))
        setImageDefault(getImageEmoticonSmile());
    else if (value == wxString(":("))
        setImageDefault(getImageEmoticonAngry());
    else if (value == wxString(":|"))
        setImageDefault(getImageEmoticonWhatever());
    else if (value == wxString("<-"))
        setImageDefault(getImageEmoticonArrowLeft());
    else if (value == wxString("->"))
        setImageDefault(getImageEmoticonArrowRight());
    else if (value == wxString("<>"))
        setImageDefault(getImageEmoticonStop());

    Update();
    Refresh();
}


/////////////////////////////////////////////////////////////////////////////
//
// TerminalTxEditor
//
/////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(TerminalTxEditor, wxTextCtrl)
    EVT_CHAR(TerminalTxEditor::OnChar)
END_EVENT_TABLE()


void TerminalTxEditor::OnChar(wxKeyEvent& event)
{
    //##There are problems with the weird keys (delte, backspace, etc.), both when the user input texts and
    //when there are received characters from the serial port...

	wxChar wchr = event.GetUnicodeKey();
    //##Near future: Emulate standard terminal types:
    //##Improve this, and make this "filter" configurable from the XML config file
    //(there are still problems with pageUp, pageDown, delete, etc...):
    //##Add a line based terminal also (which sends the line only when some special char is pressed)...
//    if ((wchr != WXK_BACK) &&
//        (wchr != WXK_ESCAPE) &&
//        (wchr != WXK_DELETE) &&
//        (wchr != WXK_LEFT) &&
//        (wchr != WXK_RIGHT) &&
//        (wchr != WXK_UP) &&
//        (wchr != WXK_LEFT)
//        //(wchr != WXK_TAB)
//       )

//    if (    ((wchr >= 'a') && (wchr <= 'z')) ||
//            ((wchr >= 'A') && (wchr <= 'Z')) ||
//            ((wchr >= '0') && (wchr <= '9')) ||
//            (wchr == WXK_RETURN) ||
//            (wchr == WXK_SPACE) ||
////            (wchr == 'á') ||
////            (wchr == 'Á') ||
////            (wchr == 'é') ||
////            (wchr == 'É') ||
////            (wchr == 'í') ||
////            (wchr == 'Í') ||
////            (wchr == 'ó') ||
////            (wchr == 'Ó') ||
////            (wchr == 'ú') ||
////            (wchr == 'Ú') ||
//            //##Add Portuguese chars too...
//            (wchr == '(') ||
//            (wchr == ')') ||
//            (wchr == '.') ||
//            (wchr == ':') ||
//            (wchr == ',') ||
//            (wchr == ';') ||
//            (wchr == '=') ||
//            (wchr == '-') ||
//            (wchr == '+') ||
//            (wchr == '*') ||
//            (wchr == '/') ||
//            (wchr == wxFileName::GetPathSeparator()) ||
//            (wchr == '<') ||
//            (wchr == '>') ||
//            (wchr == '{') ||
//            (wchr == '}') ||
//            (wchr == '[') ||
//            (wchr == ']') ||
//            (wchr == '^') ||
//            (wchr == '~') ||
//            (wchr == '?') ||
////            (wchr == '¿') ||
//            (wchr == '!') ||
////            (wchr == '¡') ||
//            (wchr == '|') ||
////            (wchr == '°') ||
//            (wchr == '#') ||
//            (wchr == '"') ||
//            (wchr == '$') ||
//            (wchr == '%') ||
//            (wchr == '&')
////            (wchr == '°') ||
////            (wchr == '¬') ||
////            (wchr == '¨')
//       )
//    {

        if (mode == commModeSerial)
        {
            char *chr = (char*)&wchr; //##See if this type conversion can be improved...
            if (serialPort)
                serialPort->Write(chr, 1);
        }
        else if (mode == commModeUsbHid)
        {
            char buffer[8];
            buffer[0] = 0xB1;
            buffer[1] = *((char*)&wchr);
            for (int i = 2; i < 8; i++)
                buffer[i] = 0;
            rawhid_send(0, buffer, sizeof(buffer), 100);
        }

        //AppendText('j'); //##Debug

        //There was a bug in the XDFTerminal, corrected with this call (here it seems tha the bug is not present
        //but just in case -because it was an old underdocumented bug, and this line doesn't hurt-):
        SetDefaultStyle(wxTextAttr(getTxColour(), GetBackgroundColour()));
//    }
    event.Skip();
}


/////////////////////////////////////////////////////////////////////////////
//
// TerminalRXThread
//
/////////////////////////////////////////////////////////////////////////////
int TerminalRXThread::readSerial(char *buffer, size_t sizeOfBuffer)
{
    if (serialPort == NULL)
        return 0;

    return serialPort->Read(buffer, sizeOfBuffer-1);

//##Future: Implement this to read and show all the port's lines:
//			//Port lines:
//			int flags = serialPort.GetLineState();
//			chkCTS->SetValue(flags & wxSERIAL_LINESTATE_CTS);
//			chkDSR->SetValue(flags & wxSERIAL_LINESTATE_DSR);
//			chkRING->SetValue(flags & wxSERIAL_LINESTATE_RING);
//			chkDCD->SetValue(flags & wxSERIAL_LINESTATE_DCD);
		//Check if any Raw HID packet has arrived
}


int TerminalRXThread::readUsbHid(char *buffer, size_t sizeOfBuffer)
{
    int i = 0;
    int num = rawhid_recv(0, buffer, sizeOfBuffer, 220); //##Unhardcode device number...

    if (num < 0)
    {
        //## printf("\nError reading; device went offline\n");
        rawhid_close(0);
        return 0;
    }
    if (num > 0)
    {
        //printf("\nrecv %d bytes:\n", num);
        switch(buffer[0] & 255)
        {
            case 0xB1:
                for (i = 1; i < num; i++)
                {
                    if (buffer[i] > 0)
                        buffer[i - 1] = buffer[i]; //Removes the first (code) byte from the buffer...
                    else
                        break;
                }
                break;

            //## default:
                //## printf("Unhandled header received: 0x%02x\n", buffer[0]&255);
        }
    }
    //##return i; //Returns the real number of useful bytes.
    return i-1; //Returns the real number of useful bytes.
}


wxThread::ExitCode TerminalRXThread::Entry()
{
    //Buffer for received data:
    //char buffer[20] //##Future: Make buffer size configurable...
    char buffer[8];

    unsigned int readCount = 0;

    while(!TestDestroy())
    {
        if (mode == commModeUsbHid)
            readCount = readUsbHid(buffer, sizeof(buffer));
        else if (mode == commModeSerial)
            readCount = readSerial(buffer, sizeof(buffer));
        else
            readCount = 0;

        if ( (readCount < sizeof(buffer)) && (readCount > 0) )
        {
            buffer[readCount] = '\0';

#if defined (linux)
            wxMutexGuiEnter(); //##Test this a lot under Windows.
#endif
            //Editor belonging to split editor terminals:
            if (rxEditor)
            {
                rxEditor->AppendText(wxConvLocal.cMB2WC(buffer));
            }

            //Editor belonging to single editor terminals:
            if (rxTxEditor)
            {
                //Changes the color to the "rx color":
                rxTxEditor->SetDefaultStyle(wxTextAttr(rxTxEditor->getRxColour(), rxTxEditor->GetBackgroundColour()));

                //Adds the text:
                rxTxEditor->AppendText(wxConvLocal.cMB2WC(buffer));

                //Restores the color to the "tx color":
                rxTxEditor->SetDefaultStyle(wxTextAttr(rxTxEditor->getTxColour(), rxTxEditor->GetBackgroundColour()));
            }

            //Emoticons support (with separated emoticons window):
            if (emoticonScreen)
            {
                //As this is for separated emotincon windows, shows only the last received emoticon:
                //(##in future implementations, it will be possible to insert the emoticons inside the
                //rxEditor itself, between the text):

                //First: Find the last emoticon:
                wxString strToParse(buffer);
                int index =             strToParse.Find(":)");  //##Future: unhardcode these
                index = std::max(index, strToParse.Find(":(")); //comparisons... Create a method
                index = std::max(index, strToParse.Find(":|")); //to send to this class the list of
                index = std::max(index, strToParse.Find("<-")); //valid emoticons.
                index = std::max(index, strToParse.Find("->"));
                index = std::max(index, strToParse.Find("<>"));

                //Only sets the last emoticon, nochange otherwise:
                if (index != wxNOT_FOUND)
                {
                    if (index < (int)(strToParse.Len() - 1))
                    {
                        emoticonScreen->setEmoticonStr( wxString(strToParse[index]) +
                                                        wxString(strToParse[index + 1]));
                    }
                }
            }

#if defined (linux)
            wxMutexGuiLeave(); //##Test this a lot under Windows.
#endif
            //wxThread::Sleep(10); //Is this necessary? Not by now, but have to do more testing.
        }
    }

    return NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
// TerminalCommManager
//
/////////////////////////////////////////////////////////////////////////////
TerminalCommManager::TerminalCommManager() :    portName(wxString("")),
                                                mode(commModeSerial),
                                                //serialPortDCS

                                                baudRate(wxBAUD_115200),

                                                usbVid(0), //##Check if this is a good practice for USB devices...
                                                usbPidApp(0),
                                                usbPidBoot(0),
                                                usbDeviceNumber(0),

                                                serialPort(NULL),

                                                rxThread(NULL),

                                                splitTerminal(NULL),
                                                singleTerminal(NULL)
{
    serialPort = new wxSerialPort();
}


TerminalCommManager::~TerminalCommManager()
{
    close();
    if (serialPort)
    {
        delete serialPort;
        serialPort = NULL;
    }
}


void TerminalCommManager::setMode(CommMode value)
{
    mode = value;
    if (singleTerminal)
        if (singleTerminal->getRxTxEditor())
            singleTerminal->getRxTxEditor()->setMode(value);
//##
//    if (splitTerminal)
//        if (splitTerminal->getTxEditor())
//            splitTerminal->getTxEditor()->setMode(value);
}


void TerminalCommManager::setPortName(const wxString& value)
{
    portName = value;

    //##Future: :
    if (value == wxString("HID")) //##Unhardcode...
    {
        setMode(commModeUsbHid);
        setUsbVid(0x03EB); //##Unhardcode...
        setUsbPidBoot(0x2067); //##Unhardcode...
        setUsbPidApp(0x204F); //##Unhardcode...
        setUsbDeviceNumber(1); //##See if this is correct... ##Unhardcode...
    }
    else
        setMode(commModeSerial);
}


bool TerminalCommManager::openUsbHid()
{
    int r;
    unsigned char bootPacket[130];

    bootPacket[0] = 0xFF;
    bootPacket[1] = 0xFF;
    memset(bootPacket + 2, 0, sizeof(bootPacket) - 2);

    //First, try to open in console mode:
    //##r = rawhid_open(1, 0x03EB, 0x204F, 0xFF00, 0x01);
    r = rawhid_open(getUsbDeviceNumber(), getUsbVid(), getUsbPidApp(), 0xFF00, 0x01);
    if (r <= 0)
    {
        //Could not open:
        //## printf("No DuinoBot HID device found\n"); //##Future: Pass all this to the notifier (messages Window)...

        //Try to open the bootloader:
        //##r = rawhid_open(1, 0x03EB, 0x2067, 0xFFDC, 0xFB);
        r = rawhid_open(getUsbDeviceNumber(), getUsbVid(), getUsbPidBoot(), 0xFFDC, 0xFB);
        if (r <= 0)
        {
            //Dit not found console, nor bootloader:
            //## printf("No DuinoBot HID Hardware found");
            return false;
        }
        else
        {
            //If bootloader found, sends the reset command, so it starts the application:
            //## printf("Bootloader found. Jumping to application\n");
            rawhid_send(0, bootPacket, sizeof(bootPacket), 25);
            rawhid_close(0);

            //##while ( (r = rawhid_open(1, 0x03EB, 0x204F, 0xFF00, 0x01)) <= 0 )
            while ( rawhid_open(getUsbDeviceNumber(), getUsbVid(), getUsbPidApp(), 0xFF00, 0x01) <= 0 )
            {
                //## printf(".");
#if defined (WIN32)
                Sleep(1000);
#else
                wxMilliSleep(1000);
#endif
                rawhid_send(0, bootPacket, sizeof(bootPacket), 25);
                rawhid_close(0);
            }
            //## printf("\n");
        }
    }
    //## printf("DuinoBot 1.x HID found\n");
    return true;
}


bool TerminalCommManager::openSerial()
{
    if (serialPort == NULL)
        return false;

    if (!serialPort->IsOpen())
    {
        serialPort->Open(portName);

        //Very important: It seems that with the wxSerialPort class, the baudrate must be set with the
        //open port. Doing this with the closed port has no effect:
        serialPort->SetBaudRate(baudRate);
        if (!serialPort->IsOpen())
        {
            //There were errors:
            serialPort->Close();
            return false;
        }
    }
    else
    {
        //If the port was already open, do nothing:
        return false;
    }
    return true;
}


bool TerminalCommManager::open()
{
    bool channelOpen = false;

    if (getMode() == commModeUsbHid)
    {
        channelOpen = openUsbHid();
    }
    else if (getMode() == commModeSerial)
    {
        channelOpen = openSerial();
    }

    //Start the thread, enable and disable GUI buttons, etc.:
    if (channelOpen)
    {
        wxTextCtrl *rxEditor = NULL;
        EmoticonScreen *rxEmoticon = NULL;
        if (getSplitTerminal())
        {
            rxEditor = getSplitTerminal()->getRxEditor();
            rxEmoticon = getSplitTerminal()->getRxEmoticon();
        }
        TerminalTxEditor *rxTxEditor = NULL;
        if (getSingleTerminal())
            rxTxEditor = getSingleTerminal()->getRxTxEditor();
        rxThread = new TerminalRXThread(getMode(), serialPort, rxEditor, rxTxEditor, rxEmoticon);
        if (rxThread)
        {
            if ( rxThread->Create() == wxTHREAD_NO_ERROR )
            {
                if ( rxThread->Run() == wxTHREAD_NO_ERROR )
                {
                    enableTerminalsButtons(false, true);
                    return true;
                }
            }
        }
    }

    //Errors:
    return false;
}


void TerminalCommManager::close()
{
    if (rxThread)
    {
        //rxThread->Wait(); //##This is in theory what has to be done, but DOES NOT WORK! And the wx
                            //documentation is not really much clear regarging this.
        rxThread->Delete(); //##This must not be used with joinable threads, but it works.
        rxThread = NULL;
    }
    if (serialPort)
    {
        if (serialPort->IsOpen()) //##Is this if() necessary?
            serialPort->Close();
    }
    enableTerminalsButtons(true, false);
}


void TerminalCommManager::setBaudRate(wxBaud value)
{
    baudRate = value;
    if (serialPort)
        serialPort->SetBaudRate(baudRate);
}


void TerminalCommManager::setUsbVid(int value)
{
    //##Change the real value too:
    usbVid = value;
}


void TerminalCommManager::setUsbPidApp(int value)
{
    //##Change the real value too:
    usbPidApp = value;
}


void TerminalCommManager::setUsbPidBoot(int value)
{
    //##Change the real value too:
    usbPidBoot = value;
}


void TerminalCommManager::setUsbDeviceNumber(int value)
{
    //##Change the real value too:
    usbDeviceNumber = value;
}


void TerminalCommManager::enableTerminalsButtons(bool openEnable, bool closeEnable)
{
    if (getSplitTerminal())
        getSplitTerminal()->enableTerminalsButtons(openEnable, closeEnable);
    if (getSingleTerminal())
        getSingleTerminal()->enableTerminalsButtons(openEnable, closeEnable);
}


/////////////////////////////////////////////////////////////////////////////
//
// BaseTerminalGUI
//
/////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(BaseTerminalGUI, BubblePanel)
    EVT_CHECKBOX(ID_CheckShowEmoticons, BaseTerminalGUI::OnCheckShowEmoticonsClick)
END_EVENT_TABLE()


BaseTerminalGUI::BaseTerminalGUI(   wxWindow* parent,
                                    wxWindowID id,
                                    Bubble *bubble,
                                    TerminalCommManager *commManager,
                                    bool showConfiguration,
                                    const wxColour& colour,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    const wxString& name) : BubblePanel(parent,
                                                                        id,
                                                                        colour,
                                                                        true,
                                                                        pos,
                                                                        size,
                                                                        style,
                                                                        name),
                                                            bubble(bubble),
                                                            commManager(commManager),
                                                            showConfiguration(showConfiguration),
                                                            sizerMain(NULL),
                                                            sizerEditors(NULL),
                                                            sizerRx(NULL),
                                                            sizerButtons(NULL),
                                                            panelEditors(NULL),
                                                            panelButtons(NULL),
                                                            panelPortStatus(NULL),
                                                            buttonOpenPort(NULL),
                                                            buttonClosePort(NULL),
                                                            buttonClearAll(NULL),
                                                            checkShowEmoticons(NULL),

                                                            rxColour(*wxGREEN), //##Get this from XML configu file.
                                                            txColour(*wxBLUE) //##Get this from XML configu file.
{
//##There will be a configuration panel in the future, not one in this base class (like the
//current "Show emoticons" checkBox). So this kind of thing is temporal.

//For some unknown reason, this works different for Linux and Windows:
#if defined (WIN32)
    sizerMain = new wxFlexGridSizer(1, 3);
#else
    //##Future: Test this on Mac OS X:
    sizerMain = new wxFlexGridSizer(3, 1, 0, 0);
#endif
    panelEditors = new BubblePanel(this, wxNewId(), GetBackgroundColour(), true);
    panelButtons = new BubblePanel(this, wxNewId(), GetBackgroundColour(), true);
    panelPortStatus = new BubblePanel(this, wxNewId(), GetBackgroundColour(), true);

    if (sizerMain)
    {
        sizerMain->AddGrowableRow(0);
        sizerMain->AddGrowableCol(0);

        SetSizer(sizerMain);
        if (panelEditors)
        {
            sizerMain->Add(panelEditors, wxSizerFlags().Expand().Bottom());
        }
        if (panelButtons)
        {
#if defined (WIN32)
            sizerButtons = new wxFlexGridSizer(3, 2);
#else
            //##Future: Test this on Mac OS X:
            sizerButtons = new wxFlexGridSizer(2, 3, 0, 0);
#endif
            if (sizerButtons)
            {
                panelButtons->SetSizer(sizerButtons);
                sizerButtons->AddGrowableRow(0);
                sizerButtons->AddGrowableRow(1);
                sizerButtons->AddGrowableCol(0);
#if defined (WIN32)
                sizerButtons->AddGrowableCol(1);
                sizerButtons->AddGrowableCol(2);
#endif

                buttonOpenPort = new wxButton(panelButtons, wxNewId(), _("Open"));
                if (buttonOpenPort)
                {
                    sizerButtons->Add(buttonOpenPort, wxSizerFlags().Expand().Bottom());
                    buttonOpenPort->Connect(   wxEVT_COMMAND_BUTTON_CLICKED,
                                                wxCommandEventHandler(BaseTerminalGUI::onButtonOpenPort),
                                                NULL,
                                                this
                                            );
                }
                buttonClosePort = new wxButton(panelButtons, wxNewId(), _("Close"));

                if (buttonClosePort)
                {
                    sizerButtons->Add(buttonClosePort, wxSizerFlags().Expand().Bottom());
                    buttonClosePort->Connect(   wxEVT_COMMAND_BUTTON_CLICKED,
                                                wxCommandEventHandler(BaseTerminalGUI::onButtonClosePort),
                                                NULL,
                                                this
                                            );
                    buttonClosePort->Enable(false);
                }
                buttonClearAll = new wxButton(panelButtons, wxNewId(), _("Clear all"));
                if (buttonClearAll)
                {
                    sizerButtons->Add(buttonClearAll, wxSizerFlags().Expand().Bottom());
                    buttonClearAll->Connect(   wxEVT_COMMAND_BUTTON_CLICKED,
                                                wxCommandEventHandler(BaseTerminalGUI::onButtonClearAll),
                                                NULL,
                                                this
                                            );
                }
                if (showConfiguration)
                {
                    checkShowEmoticons = new wxCheckBox(panelButtons, ID_CheckShowEmoticons, _("Show emoticons"));
                    if (checkShowEmoticons)
                    {
                        checkShowEmoticons->SetValue(true); //##This will be taken from a config file...
                        sizerButtons->Add(checkShowEmoticons);
                    }
                }
                sizerButtons->Fit(panelButtons);
            }
            sizerMain->Add(panelButtons, wxSizerFlags().Expand().Bottom());
        }
        if (panelPortStatus)
        {
            sizerMain->Add(panelPortStatus, wxSizerFlags().Expand().Bottom());
        }
        sizerMain->Fit(this);
    }
}


BaseTerminalGUI::~BaseTerminalGUI()
{
}


void BaseTerminalGUI::updateGUI()
{
    if (buttonOpenPort)
        buttonOpenPort->SetLabel(_("Open"));
    if (buttonClosePort)
        buttonClosePort->SetLabel(_("Close"));
    if (buttonClearAll)
        buttonClearAll->SetLabel(_("Clear all"));
    if (checkShowEmoticons)
        checkShowEmoticons->SetLabel(_("Show emoticons"));
}


void BaseTerminalGUI::onButtonOpenPort(wxCommandEvent& event)
{
    if (commManager)
    {
        if (bubble)
            commManager->setPortName(bubble->getBootPortName()); //##This will be configurable...
        commManager->setBaudRate(wxBAUD_115200); //##This will be configurable...
        commManager->open();
    }
}


void BaseTerminalGUI::onButtonClosePort(wxCommandEvent& event)
{
    if (commManager)
    {
        commManager->close();
    }
}


void BaseTerminalGUI::onButtonClearAll(wxCommandEvent& event)
{
    if (getCommManager())
    {
        if (getCommManager()->getSplitTerminal())
            getCommManager()->getSplitTerminal()->clear();
        if (getCommManager()->getSingleTerminal())
            getCommManager()->getSingleTerminal()->clear();
    }
}


void BaseTerminalGUI::enableTerminalsButtons(bool openEnable, bool closeEnable)
{
    if (buttonOpenPort)
        buttonOpenPort->Enable(openEnable);
    if (buttonClosePort)
        buttonClosePort->Enable(closeEnable);
}


/////////////////////////////////////////////////////////////////////////////
//
// SingleTerminalGUI
//
/////////////////////////////////////////////////////////////////////////////
SingleTerminalGUI::SingleTerminalGUI(   wxWindow* parent,
                                        wxWindowID id,
                                        Bubble *bubble,
                                        TerminalCommManager *commManager,
                                        const wxColour& colour,
                                        const wxPoint& pos,
                                        const wxSize& size,
                                        long style,
                                        const wxString& name) : BaseTerminalGUI(parent,
                                                                                id,
                                                                                bubble,
                                                                                commManager,
                                                                                false,
                                                                                colour,
                                                                                pos,
                                                                                size,
                                                                                style,
                                                                                name),
                                                                brotherTerminal(NULL),
                                                                rxTxEditor(NULL)
{
    if (getCommManager())
    {
        wxSerialPort *serial = getCommManager()->getSerialPort();
        if (panelEditors)
        {
            rxTxEditor = new TerminalTxEditor(  serial,
                                                panelEditors,
                                                wxNewId()
                                             );
#if defined (WIN32)
            sizerEditors = new wxFlexGridSizer(1, 1);
#else
            sizerEditors = new wxFlexGridSizer(1, 1, 0, 0);
#endif
            if (sizerEditors)
            {
                panelEditors->SetSizer(sizerEditors);
                sizerEditors->AddGrowableRow(0);
                sizerEditors->AddGrowableCol(0);
                if (rxTxEditor)
                {
                    getCommManager()->setSingleTerminal(this);

                    rxTxEditor->SetBackgroundColour(*wxBLACK); //##Make this configurable.
                    rxTxEditor->SetDefaultStyle(wxTextAttr(getTxColour(), rxTxEditor->GetBackgroundColour()));
                    sizerEditors->Add(rxTxEditor, wxSizerFlags().Expand().Bottom());
                }
                sizerEditors->Fit(panelEditors);
            }
        }
    }
}


SingleTerminalGUI::~SingleTerminalGUI()
{
}


void SingleTerminalGUI::clear()
{
    if (getRxTxEditor())
        getRxTxEditor()->Clear();
}


void SingleTerminalGUI::setRxColour(const wxColour& value)
{
    rxColour = value;
    if (rxTxEditor)
        rxTxEditor->setRxColour(value);
}


void SingleTerminalGUI::setTxColour(const wxColour& value)
{
    txColour = value;
    if (rxTxEditor)
        rxTxEditor->setTxColour(value);
}


/////////////////////////////////////////////////////////////////////////////
//
// SplitTerminalGUI
//
/////////////////////////////////////////////////////////////////////////////
SplitTerminalGUI::SplitTerminalGUI( wxWindow* parent,
                                    wxWindowID id,
                                    Bubble *bubble,
                                    TerminalCommManager *commManager,
                                    const wxColour& colour,
                                    const wxPoint& pos,
                                    const wxSize& size,
                                    long style,
                                    const wxString& name) : BaseTerminalGUI(parent,
                                                                            id,
                                                                            bubble,
                                                                            commManager,
                                                                            true,
                                                                            colour,
                                                                            pos,
                                                                            size,
                                                                            style,
                                                                            name),
                                                            brotherTerminal(NULL),
                                                            rxPanel(NULL),
                                                            rxEmoticon(NULL),
                                                            rxEditor(NULL),
                                                            txEditor(NULL),
                                                            splitRxTx(NULL)
{
    if (getCommManager())
    {
        wxSerialPort *serial = getCommManager()->getSerialPort();
        if (panelEditors)
        {
            splitRxTx = new wxSplitterWindow(   panelEditors, wxNewId(),
                                                wxDefaultPosition, wxDefaultSize,
                                                wxSP_NOBORDER,
                                                wxString("splitRxTx")
                                            );
            if (splitRxTx)
            {
                splitRxTx->SetMinimumPaneSize(30);
                splitRxTx->SetSashGravity(0.5);

                txEditor = new TerminalTxEditor(serial,
                                                splitRxTx,
                                                wxNewId()
                                               );
                rxPanel = new BubblePanel(  splitRxTx,
                                            wxNewId(),
                                            GetBackgroundColour(),
                                            true
                                         );
                if (rxPanel)
                {
                    rxEditor = new wxTextCtrl(  rxPanel,
                                                wxNewId(),
                                                wxString(""),
                                                pos, size,
                                                wxHSCROLL |
                                                wxTE_MULTILINE |
                                                wxTE_RICH2 |
                                                wxTE_READONLY
                                             );
                    rxEmoticon = new EmoticonScreen(rxPanel,
                                                    wxNewId()
                                                   );
                #if defined (WIN32)
                    sizerEditors = new wxFlexGridSizer(1, 1);
                #else
                    sizerEditors = new wxFlexGridSizer(1, 1, 0, 0);
                #endif
                    if (sizerEditors)
                    {
                        panelEditors->SetSizer(sizerEditors);
                        sizerEditors->AddGrowableRow(0);
                        sizerEditors->AddGrowableCol(0);
                        sizerEditors->Add(splitRxTx, wxSizerFlags().Expand().Bottom());
                        if (txEditor && rxEditor && rxEmoticon && rxPanel)
                        {
                            getCommManager()->setSplitTerminal(this);

                            splitRxTx->SplitHorizontally(txEditor, rxPanel);

                            txEditor->SetBackgroundColour(*wxBLACK); //##Make this configurable.
                            txEditor->SetDefaultStyle(wxTextAttr(getTxColour(), txEditor->GetBackgroundColour()));

                            rxEditor->SetBackgroundColour(*wxBLACK); //##Make this configurable.
                            rxEditor->SetDefaultStyle(wxTextAttr(getRxColour(), rxEditor->GetBackgroundColour()));

                            #if defined (WIN32)
                                sizerRx = new wxFlexGridSizer(2, 1);
                            #else
                                sizerRx = new wxFlexGridSizer(1, 2, 0, 0);
                            #endif
                            if (sizerRx)
                            {
                                rxPanel->SetSizer(sizerRx);
                                sizerRx->AddGrowableRow(0);
                                sizerRx->AddGrowableCol(0);
                                sizerRx->AddGrowableCol(1);
                                sizerRx->Add(rxEditor, wxSizerFlags().Expand().Bottom());
                                sizerRx->Add(rxEmoticon, wxSizerFlags().Expand().Bottom());
                                sizerRx->Fit(rxPanel);
                            }
                        }
                        sizerEditors->Fit(panelEditors);
                    }
                    //splitRxTx->SetSashPosition(100); //##Get this from the config file!
                }
            }
        }
    }
}


SplitTerminalGUI::~SplitTerminalGUI()
{
}


void SplitTerminalGUI::clear()
{
    if (getRxEditor())
        getRxEditor()->Clear();
    if (getTxEditor())
        getTxEditor()->Clear();
}


void SplitTerminalGUI::setRxColour(const wxColour& value)
{
    rxColour = value;
    if (rxEditor)
        rxEditor->SetDefaultStyle(wxTextAttr(value, rxEditor->GetBackgroundColour()));
}


void SplitTerminalGUI::setTxColour(const wxColour& value)
{
    txColour = value;
    if (txEditor)
        txEditor->setTxColour(value);
}


bool SplitTerminalGUI::getEmoticonsEnabled() const
{
    if (rxEmoticon)
        return rxEmoticon->IsShown();
    return false;
}


void SplitTerminalGUI::setEmoticonsEnabled(bool value)
{
    if (rxEmoticon)
    {
        rxEmoticon->Show(value);
        if (rxEmoticon->IsShown())
            sizerRx->Add(rxEmoticon, wxSizerFlags().Expand().Bottom());
        else
            sizerRx->Detach(rxEmoticon);
    }

    //This is a small trick to force the sizers to refresh correctly once the rxEmoticon has been
    //hidden or shown:
    if (splitRxTx)
    {
        splitRxTx->SetSashPosition(splitRxTx->GetSashPosition() + 1);
        splitRxTx->SetSashPosition(splitRxTx->GetSashPosition() - 1);
    }
    Update();
    Refresh();
}


void SplitTerminalGUI::OnCheckShowEmoticonsClick(wxCommandEvent &event)
{
    if (checkShowEmoticons)
        setEmoticonsEnabled(checkShowEmoticons->GetValue());
}
