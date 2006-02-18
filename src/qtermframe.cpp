/*******************************************************************************
FILENAME:      qtermframe.cpp
REVISION:      2001.10.4 first created.

AUTHOR:        kingson fiasco
*******************************************************************************/
/*******************************************************************************
                                    NOTE
 This file may be used, distributed and modified without limitation.
 *******************************************************************************/

#include "qtermwindow.h"
#include "qtermframe.h"
#include "qtermwndmgr.h"
#include "qtermtimelabel.h"
#include "qtermconfig.h"
#include "qterm.h"
#include "qtermparam.h"
#include "qtermtoolbutton.h"
#include "statusBar.h"
//Added by qt3to4:
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFrame>
#include <QEvent>
#include <QByteArray>
#include <QCloseEvent>
#include <QTime>

#include "aboutdialog.h"
#include "addrdialog.h"
#include "prefdialog.h"
#include "quickdialog.h"
#include "keydialog.h"
#include "trayicon.h"
#include "imageviewer.h"

#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
#include <unistd.h>
#endif

#include <stdlib.h>
#include <stdio.h>

// #include <qtooltip.h>
#include <QTextCodec>
#include <QApplication>
#include <QWorkspace>
#include <QToolBar>
#include <QToolButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPixmap>
#include <QMenu>
#include <QMenuBar>
#include <QTabBar>
#include <QMessageBox>
#include <QRegExp>
// #include <qtextcodec.h>
// #include <qapplication.h>
// #include <qworkspace.h>
// #include <q3toolbar.h>
// #include <qtoolbutton.h>
// #include <q3vbox.h>
// #include <qpixmap.h>
// #include <q3popupmenu.h>
// #include <qmenubar.h>
// #include <qtabbar.h>
// #include <qstatusbar.h>
// #include <qmessagebox.h>
// #include <q3hbox.h>
// #include <qregexp.h>

// #if QT_VERSION < 300
// #include <qplatinumstyle.h>
// #include <qmotifstyle.h>
// #include <qmotifplusstyle.h>
// #include <qcdestyle.h>
// #include <qsgistyle.h>
// #else
// #include <qstylefactory.h>
// #include <qstyle.h>
// #endif
#include <QStyle>
#include <QStyleFactory>
#include <QFont>
#include <QFontDialog>
#include <QPalette>
#include <QDir>
#include <QStringList>
#include <QListWidget>
#include <QLabel>
#include <QMovie>
// #include <qfont.h>
// #include <qfontdialog.h>
// #include <qtranslator.h>
// #include <qpalette.h>
// #include <qdir.h>
// #include <qstringlist.h>
// #include <q3listbox.h>
// #include <qlabel.h>
// #include <qmovie.h>
#include <QLineEdit>
// #include <QAction>
#include <QInputDialog>
#include <QStatusBar>

// #include <qlineedit.h>
// #include <q3accel.h>
// #include <qaction.h>
// #include <qinputdialog.h>

extern QString fileCfg;
extern QString addrCfg;

extern QString pathLib;
extern QString pathPic;
extern QString pathCfg;

extern void clearDir(const QString& );
extern QStringList loadNameList(QTermConfig *);
extern bool loadAddress(QTermConfig *, int, QTermParam &);
extern void saveAddress(QTermConfig *, int, const QTermParam &);



//constructor
QTermFrame::QTermFrame()
    : QMainWindow( 0, Qt::WDestructiveClose )
{
//set the layout
	QFrame * vb = new QFrame(this);
	QVBoxLayout * vbLayout = new QVBoxLayout(vb);
	vbLayout->setMargin(0);
// 	Q3VBox *vb = new Q3VBox( this );
// 	FIXME:QFrame?
	vb->setFrameStyle( QFrame::StyledPanel | QFrame::Sunken );
	setCentralWidget( vb );
	ws = new QWorkspace( vb );
	vbLayout->addWidget(ws);

	tray = 0;
	trayMenu = 0;
	
//set menubar
	addMainMenu();

//setup toolbar
	addMainTool();

// add the custom defined key
	updateKeyToolBar();

// diaable some menu & toolbar
	enableMenuToolBar( false );

// 
	connect(this, SIGNAL(toolBarPositionChanged(QToolBar*)), 
			this, SLOT(toolBarPosChanged(QToolBar*)));
//!!!create a horizonal layout to hold the tabbar,the reason of why not put
//the tabbar directly on the status bar is when no tab in the tabbar,the
//statusbar display a horizonal line,ugly.
//perhaps there is some function in statusbar to solve this.
	QWidget * hb = new QWidget(statusBar());
	QHBoxLayout * hbLayout = new QHBoxLayout(hb);
	hbLayout->setMargin(0);
// 	Q3HBox * hb=new Q3HBox(statusBar());
	statusBar()->addWidget(hb,90);
//create a tabbar in the hbox
	tabBar=new QTabBar(hb);
	hbLayout->addWidget(tabBar);
	connect(tabBar,SIGNAL(selected(int)),this,SLOT(selectionChanged(int)));
	//tabBar->setShape(QTabBar::TriangularBelow);
	tabBar->setShape(QTabBar:: RoundedSouth);

//create a progress bar to notify the download process
	m_pStatusBar = new QTerm::StatusBar(statusBar(), "mainStatusBar");
	statusBar()->addWidget(m_pStatusBar, 0, false);

//create the window manager to deal with the window-tab-icon pairs
	wndmgr=new QTermWndMgr(this);

// expressly connect sites in addressbook
// 	Q3Accel *accel2 = new Q3Accel(this);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_1, 0);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_2, 1);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_3, 2);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_4, 3);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_5, 4);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_6, 5);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_7, 6);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_8, 7);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_9, 8);
// 	accel2->insertItem(Qt::CTRL+Qt::ALT+Qt::Key_9, 9);
// 	connect( accel2, SIGNAL(activated(int)), 
// 	this,SLOT(connectMenuActivated(int)));
// 
// // ALT+# to switch between 10 windows
// 	Q3Accel *accel = new Q3Accel(this);
// 	accel->insertItem(Qt::ALT+Qt::Key_1, 1);
// 	accel->insertItem(Qt::ALT+Qt::Key_2, 2);
// 	accel->insertItem(Qt::ALT+Qt::Key_3, 3);
// 	accel->insertItem(Qt::ALT+Qt::Key_4, 4);
// 	accel->insertItem(Qt::ALT+Qt::Key_5, 5);
// 	accel->insertItem(Qt::ALT+Qt::Key_6, 6);
// 	accel->insertItem(Qt::ALT+Qt::Key_7, 7);
// 	accel->insertItem(Qt::ALT+Qt::Key_8, 8);
// 	accel->insertItem(Qt::ALT+Qt::Key_9, 9);
// // ALT+->(<-) to active next(previous) window
// 	accel->insertItem(Qt::ALT+Qt::Key_Left, 200);
// 	accel->insertItem(Qt::ALT+Qt::Key_Up, 200);
// 	accel->insertItem(Qt::ALT+Qt::Key_Right, 201);
// 	accel->insertItem(Qt::ALT+Qt::Key_Down, 201);
// 
// 	connect( accel, SIGNAL(activated(int)), this, SLOT(switchWin(int)) );
	
	//initialize all settings
	iniSetting();

	installEventFilter(this);
}

//destructor
QTermFrame::~QTermFrame()
{
   delete wndmgr; 
}
//initialize setting from qterm.cfg
void QTermFrame::iniSetting()
{
	QTermConfig * conf= new QTermConfig(fileCfg);

	QString strTmp;
	

	strTmp = conf->getItemValue("global","fullscreen");
	if(strTmp=="1")
	{
		m_bFullScreen = true;
		menuBar()->setItemChecked( ID_VIEW_FULL, true );
		showFullScreen();
	}
	else
	{
		m_bFullScreen = false;
	//window size
		strTmp = conf->getItemValue("global","max");
		if(strTmp=="1")
			showMaximized();
		else
		{
			const char * size = conf->getItemValue("global","size");
			if( size!=NULL )
			{
				int x,y,cx,cy;
				sscanf(size,"%d %d %d %d",&x,&y,&cx,&cy);
				resize(QSize(cx,cy));
				move(QPoint(x,y));
			}
		}
	}

	theme = conf->getItemValue("global","theme");
	QStyle * style = QStyleFactory::create(theme);
	if (style)
		qApp->setStyle(style);
	
	//language
	strTmp = conf->getItemValue("global","language");
	if(strTmp=="eng")	
		langMenu->setItemChecked( sEng, TRUE );	
	else if(strTmp=="chs")	
		langMenu->setItemChecked( sChs, TRUE );	
	else if(strTmp=="cht")	
		langMenu->setItemChecked( sCht, TRUE );	
	else
		langMenu->setItemChecked( sEng, TRUE );	

	menuBar()->setItemChecked( ID_EDIT_ESC_NO, true );
	m_strEscape = "";

	strTmp = conf->getItemValue("global","clipcodec");
	if(strTmp=="0")
	{
		m_nClipCodec=0;
		menuBar()->setItemChecked(ID_EDIT_CODEC_GBK, true);
	}
	else
	{
		m_nClipCodec=1;
		menuBar()->setItemChecked(ID_EDIT_CODEC_BIG5, true);
	}

	strTmp = conf->getItemValue("global","vscrollpos");
	if(strTmp=="0")
	{
		m_nScrollPos=0;
		menuBar()->setItemChecked(ID_VIEW_SCROLL_HIDE, true);
	}
	else if(strTmp=="1")
	{
		m_nScrollPos=1;
		menuBar()->setItemChecked(ID_VIEW_SCROLL_LEFT, true);
	}
	else
	{
		m_nScrollPos=2;
		menuBar()->setItemChecked(ID_VIEW_SCROLL_RIGHT, true);
	}

	strTmp = conf->getItemValue("global","statusbar");
	m_bStatusBar = (strTmp!="0");
	menuBar()->setItemChecked(ID_VIEW_STATUS,m_bStatusBar );

	
	strTmp = conf->getItemValue("global","switchbar");
	m_bSwitchBar = (strTmp!="0");
	menuBar()->setItemChecked(ID_VIEW_SWITCH,m_bSwitchBar );
	if(m_bSwitchBar)
		statusBar()->show();
	else
		statusBar()->hide();

	m_bBossColor = false;

	loadPref( conf );

	setUseDock(m_pref.bTray);

	delete conf;
}

void QTermFrame::loadPref( QTermConfig * conf )
{
	QString strTmp;
	strTmp = conf->getItemValue("preference","xim");
	m_pref.nXIM = strTmp.toInt();
	strTmp = conf->getItemValue("preference","wordwrap");
	m_pref.nWordWrap = strTmp.toInt();
	strTmp = conf->getItemValue("preference","smartww");
	m_pref.bSmartWW=(strTmp!="0");
	strTmp = conf->getItemValue("preference","wheel");
	m_pref.bWheel=(strTmp!="0");
	strTmp = conf->getItemValue("preference","url");
	m_pref.bUrl=(strTmp!="0");
	strTmp = conf->getItemValue("preference","logmsg");
	m_pref.bLogMsg=(strTmp!="0");
	strTmp = conf->getItemValue("preference","blinktab");
	m_pref.bBlinkTab=(strTmp!="0");
	strTmp = conf->getItemValue("preference","warn");
	m_pref.bWarn=(strTmp!="0");
	strTmp = conf->getItemValue("preference","beep");
	m_pref.nBeep=strTmp.toInt();
	m_pref.strWave = conf->getItemValue("preference","wavefile");
	strTmp = conf->getItemValue("preference","http");
	m_pref.strHttp = strTmp;
	strTmp = conf->getItemValue("preference","antialias");
	m_pref.bAA=(strTmp!="0");
	strTmp = conf->getItemValue("preference","tray");
	m_pref.bTray=(strTmp!="0");
	strTmp = conf->getItemValue("preference","playmethod");
	m_pref.nMethod=strTmp.toInt();
	strTmp = conf->getItemValue("preference","externalplayer");
	m_pref.strPlayer=strTmp;

	strTmp = conf->getItemValue("preference","clearpool");
	m_pref.bClearPool=(strTmp!="0");
	strTmp = conf->getItemValue("preference","pool");
	m_pref.strPoolPath=strTmp.isEmpty()?pathCfg+"pool/":strTmp;
	if( m_pref.strPoolPath.right(1) != "/" )
		m_pref.strPoolPath.append('/');
	strTmp = conf->getItemValue("preference","zmodem");
	m_pref.strZmPath=strTmp.isEmpty()?pathCfg+"zmodem/":strTmp;
	if( m_pref.strZmPath.right(1) != "/" )
		m_pref.strZmPath.append('/');
	strTmp = conf->getItemValue("preference","image");
	m_pref.strImageViewer=QString::fromLocal8Bit(strTmp);
}

//save current setting to qterm.cfg
void QTermFrame::saveSetting()
{
	QTermConfig * conf= new QTermConfig(fileCfg);

	QString strTmp;
	//save font
	conf->setItemValue("global","font",qApp->font().family().toLocal8Bit());
	strTmp.setNum(QFontInfo(qApp->font()).pointSize());
	conf->setItemValue("global","pointsize",strTmp);
	strTmp.setNum(QFontInfo(qApp->font()).pixelSize());
	conf->setItemValue("global","pixelsize",strTmp);

	//save window position and size
	if(isMaximized())
	{
		conf->setItemValue("global","max","1");
	}
	else
	{
		strTmp = QString("%1 %2 %3 %4").arg(x()).arg(y()).arg(width()).arg(height());
// 		cstrTmp.sprintf("%d %d %d %d",x(),y(),width(),height());
		conf->setItemValue("global","size",strTmp);
		conf->setItemValue("global","max","0");
	}
	
	if(m_bFullScreen)
		conf->setItemValue("global","fullscreen","1");
	else
		conf->setItemValue("global","fullscreen","0");
	
	// cstrTmp.setNum(theme);
	conf->setItemValue("global","theme",theme);

	int hide,dock,index,nl,extra;

	strTmp = conf->getItemValue("global", "bbsbar");
	sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
	conf->setItemValue("global","bbsbar", valueToString(mdiconnectTools->isVisibleTo(this),
							(int)dock, index, nl==1?true:false, extra));

	strTmp = conf->getItemValue("global", "keybar");
	sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
	conf->setItemValue("global","keybar", valueToString(key->isVisibleTo(this), 
							(int)dock, index, nl==1?true:false, extra));

	strTmp = conf->getItemValue("global", "mainbar");
	sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
	conf->setItemValue("global","mainbar",valueToString(mdiTools->isVisibleTo(this),
							(int)dock, index, nl==1?true:false, extra) );

	
	strTmp.setNum(m_nClipCodec);
	conf->setItemValue("global","clipcodec",strTmp);

	strTmp.setNum(m_nScrollPos);
	conf->setItemValue("global","vscrollpos",strTmp);

	conf->setItemValue("global","statusbar", m_bStatusBar?"1":"0");
	conf->setItemValue("global","switchbar", m_bSwitchBar?"1":"0");

	conf->save(fileCfg);
	delete conf;
}

//addressbook
void QTermFrame::addressBook()
{
	addrDialog addr(this,false);
	
	if(addr.exec()==1)
	{
		newWindow(addr.param, addr.ui.nameListWidget->currentRow());
	}
}
//quicklogin
void QTermFrame::quickLogin()
{
	quickDialog quick(this);
	
	QTermConfig *pConf = new QTermConfig(addrCfg);
	loadAddress(pConf, -1, quick.param);
	delete pConf;

	if(quick.exec()==1)
	{
		newWindow(quick.param);
	}
}

void QTermFrame::exitQTerm()
{
	while( wndmgr->count()>0 ) 
	{
		bool closed = ws->activeWindow()->close();
		if(!closed)
		{
			return;
		}
	}

	saveSetting();
	// clear zmodem and pool if needed
	if(m_pref.bClearPool)
	{
		clearDir(m_pref.strZmPath);
		clearDir(m_pref.strPoolPath);
		clearDir(m_pref.strPoolPath+"shadow-cache/");
	}

	setUseDock(false);
	qApp->quit();
}

//create a new display window
//QTermWindow * QTermFrame::newWindow( const QTermParam&  param, int index )
void QTermFrame::newWindow( const QTermParam&  param, int index )
{
	QTermWindow * window=new QTermWindow( this, param, index, ws,
					0,  Qt::WDestructiveClose );
	ws->addWindow(window);
	window->setCaption( param.m_strName );
	window->setIcon( QPixmap(pathLib+"pic/tabpad.png") );

	QIcon* icon=new QIcon(QPixmap(pathLib+"pic/tabpad.png"));
	//QTab *qtab=new QTab(*icon,window->caption());
	QString qtab = window->caption();
	tabBar->addTab( *icon, qtab );

	//if no this call, the tab wont display untill you resize the window
	tabBar->updateGeometry();
	tabBar->update();
	
	//add window-tab-icon to window manager
	wndmgr->addWindow(window,qtab,icon);
	
	if( ws->windowList().isEmpty() )
		window->showMaximized();
	else
	{
		ws->setFocus();
		window->show();
	}

	//activte the window-tab
//	window->setFocus();
//	wndmgr->activateTheTab(window);

//	return window;
}

//the tabbar selection changed
void QTermFrame::selectionChanged(int n)
{
	QString qtab=tabBar->tabText(n);
	wndmgr->activateTheWindow(qtab);	
}
//slot Help->About QTerm
void QTermFrame::aboutQTerm()
{
	aboutDialog about(this);
	
	about.exec();
}

//slot Help->Homepage
void QTermFrame::homepage()
{
    QString strCmd = m_pref.strHttp;
	QString strUrl = "http://qterm.sourceforge.net";

    if(strCmd.find("%L")==-1)
        strCmd += " \"" + strUrl +"\"";
    else
        strCmd.replace(QRegExp("%L",false), strUrl);

	#if !defined(_OS_WIN32_) && !defined(Q_OS_WIN32)
	strCmd += " &";
	#endif
	system(strCmd.toLocal8Bit());
}
//slot Windows menu aboutToShow
void QTermFrame::windowsMenuAboutToShow()
{
	windowsMenu->clear();
	int cascadeId = windowsMenu->insertItem(tr("Cascade"), ws, SLOT(cascade() ) );
	int tileId = windowsMenu->insertItem(tr("Tile"), ws, SLOT(tile() ) );
	if ( ws->windowList().isEmpty() ) 
	{
		windowsMenu->setItemEnabled( cascadeId, FALSE );
		windowsMenu->setItemEnabled( tileId, FALSE );
	}
	windowsMenu->insertSeparator();

#ifdef Q_OS_MACX
	// used to dock the programe
	if(isHidden())
		windowsMenu->insertItem(tr("Main Window"), this, SLOT(trayShow()));
#endif

	QWidgetList windows = ws->windowList();
	for ( int i = 0; i < int(windows.count()); ++i ) 
	{
		int id = windowsMenu->insertItem(windows.at(i)->caption(),
				this, SLOT( windowsMenuActivated( int ) ) );
		windowsMenu->setItemParameter( id, i );
		windowsMenu->setItemChecked( id, ws->activeWindow() == windows.at(i) );
	}
	
}
//slot activate the window correspond with the menu id 
void QTermFrame::windowsMenuActivated( int id )
{
	QWidget* w = ws->windowList().at( id );
	if ( w ) 
	{
		w->showNormal();
//		wndmgr->activateTheTab((QTermWindow*)w);
	} 
}

void QTermFrame::popupConnectMenu()
{
	connectMenu->clear();

	connectMenu->insertItem("Quick Login", this, SLOT(quickLogin()) );
	connectMenu->insertSeparator();
	
	QTermConfig conf(addrCfg);
	QStringList listName = loadNameList( &conf );

	for ( int i=0; i<listName.count(); i++ )
	{
		int id = connectMenu->insertItem( listName[i],
					this, SLOT(connectMenuActivated(int)));
		connectMenu->setItemParameter( id, i );
	}
	
	connectMenu->exec( connectButton->mapToGlobal( connectButton->rect().bottomLeft() ));
}
void QTermFrame::connectMenuAboutToHide()
{
	QMouseEvent me( QEvent::MouseButtonRelease, QPoint(0,0), QPoint(0,0), 
				Qt::LeftButton, Qt::NoButton);
	QApplication::sendEvent( connectButton, &me );

}
void QTermFrame::connectMenuActivated( int id )
{
	QTermConfig *pConf = new QTermConfig(addrCfg);
	QTermParam param;
	if(loadAddress(pConf, connectMenu->itemParameter(id), param))
		newWindow(param, connectMenu->itemParameter(id));
	delete pConf;
}

void QTermFrame::switchWin(int id)
{
	QWidgetList windows = ws->windowList();
	if(windows.count()==0)
		return;

	if(id==200)
	{
		wndmgr->activeNextPrev(false);
		return;
	}
	if(id==201 || id==202)
	{
		wndmgr->activeNextPrev(true);
		return;
	}

	QWidget *w = windows.at(id-1);
	if(w == ws->activeWindow() )
		return;

	if(w!=NULL)
		w->showNormal();
}

bool QTermFrame::eventFilter(QObject *o, QEvent *e)
{
/*
	if( o==this && m_pref.bTray)
	{
		if( e->type()==QEvent::ShowMinimized && m_pref.bTray )
		{
			printf("QTermFrame::eventFilter\n");
			trayHide();
			return true;
		}
	}
	return false;
*/
	return false;
}

//slot draw something e.g. logo in the background
//TODO : draw a pixmap in the background
void QTermFrame::paintEvent( QPaintEvent * )
{

}

void QTermFrame::closeEvent(QCloseEvent * clse)
{
        QWidgetList windows = ws->windowList();
        for ( int i = 0; i < int(windows.count()); ++i )
        {

                if( ( (QTermWindow *)windows.at(i) )->isConnected() )
                {
                        if (m_pref.bTray) {
                                trayHide();
                                return;
                        }
                }
        }
        while( wndmgr->count()>0 )
        {
                bool closed = ws->activeWindow()->close();
                if(!closed)
                {
                        return;
                }
        }

        saveSetting();
        // clear zmodem and pool if needed
        if(m_pref.bClearPool)
        {
                clearDir(m_pref.strZmPath);
                clearDir(m_pref.strPoolPath);
		clearDir(m_pref.strPoolPath+"shadow-cache/");

        }

        setUseDock(false);

        clse->accept();

}

void QTermFrame::langEnglish()
{
	QMessageBox::information( this, "QTerm",
			tr("This will take effect after restart,\nplease close all windows and restart."));
	QTermConfig * conf= new QTermConfig(fileCfg);
	conf->setItemValue("global","language","eng");
	conf->save(fileCfg);
	delete conf;
}

void QTermFrame::langSimplified()
{
	QMessageBox::information( this, "QTerm",
			tr("This will take effect after restart,\nplease close all windows and restart."));
	QTermConfig * conf= new QTermConfig(fileCfg);
	conf->setItemValue("global","language","chs");
	conf->save(fileCfg);
	delete conf;
}

void QTermFrame::langTraditional()
{
	QMessageBox::information( this, "QTerm",
			tr("This will take effect after restart,\nplease close all windows and restart."));	
	QTermConfig * conf= new QTermConfig(fileCfg);
	conf->setItemValue("global","language","cht");
	conf->save(fileCfg);
	delete conf;
}

void QTermFrame::connectIt()
{
	if( wndmgr->activeWindow()== NULL )
	{
		QTermParam param;
		QTermConfig *pConf = new QTermConfig(addrCfg);
		loadAddress(pConf, -1, param);
		delete pConf;
		newWindow( param );
	}
	else
		if(!wndmgr->activeWindow()->isConnected())
			wndmgr->activeWindow()->reconnect();
}
void QTermFrame::disconnect()
{
	wndmgr->activeWindow()->disconnect();
}

void QTermFrame::copy( )
{
	wndmgr->activeWindow()->copy();
}
void QTermFrame::paste( )
{
	wndmgr->activeWindow()->paste();
}
void QTermFrame::copyRect()
{
	wndmgr->activeWindow()->m_bCopyRect = !wndmgr->activeWindow()->m_bCopyRect;

	menuBar()->setItemChecked( ID_EDIT_RECT, wndmgr->activeWindow()->m_bCopyRect );

	editRect->setOn( wndmgr->activeWindow()->m_bCopyRect );

	
}
void QTermFrame::copyColor()
{
	wndmgr->activeWindow()->m_bCopyColor = !wndmgr->activeWindow()->m_bCopyColor;

	menuBar()->setItemChecked( ID_EDIT_COLOR, wndmgr->activeWindow()->m_bCopyColor );

	editColor->setOn( wndmgr->activeWindow()->m_bCopyColor );

}
void QTermFrame::copyArticle( )
{
	wndmgr->activeWindow()->copyArticle();
}

void QTermFrame::autoCopy()
{
	wndmgr->activeWindow()->m_bAutoCopy = !wndmgr->activeWindow()->m_bAutoCopy;

	menuBar()->setItemChecked( ID_EDIT_AUTO, wndmgr->activeWindow()->m_bAutoCopy );

}
void QTermFrame::wordWrap()
{
	wndmgr->activeWindow()->m_bWordWrap = !wndmgr->activeWindow()->m_bWordWrap;

	menuBar()->setItemChecked( ID_EDIT_WW, wndmgr->activeWindow()->m_bWordWrap );

}

void QTermFrame::noEsc()
{
	menuBar()->setItemChecked(ID_EDIT_ESC_NO,false);
	menuBar()->setItemChecked(ID_EDIT_ESC_ESC,false);
	menuBar()->setItemChecked(ID_EDIT_ESC_U,false);
	menuBar()->setItemChecked(ID_EDIT_ESC_CUS,false);

	m_strEscape = "";
	menuBar()->setItemChecked(ID_EDIT_ESC_NO,true); 
}
void QTermFrame::escEsc()
{
	menuBar()->setItemChecked(ID_EDIT_ESC_NO,false);
	menuBar()->setItemChecked(ID_EDIT_ESC_ESC,false);
	menuBar()->setItemChecked(ID_EDIT_ESC_U,false);
	menuBar()->setItemChecked(ID_EDIT_ESC_CUS,false);

	m_strEscape = "^[^[[";
	menuBar()->setItemChecked(ID_EDIT_ESC_ESC,true);
}
void QTermFrame::uEsc()
{
	menuBar()->setItemChecked(ID_EDIT_ESC_NO,false);
	menuBar()->setItemChecked(ID_EDIT_ESC_ESC,false);
	menuBar()->setItemChecked(ID_EDIT_ESC_U,false);
	menuBar()->setItemChecked(ID_EDIT_ESC_CUS,false);

	m_strEscape = "^u[";
	menuBar()->setItemChecked(ID_EDIT_ESC_U,true);
}
void QTermFrame::customEsc()
{
	bool ok;
	QString strEsc = QInputDialog::getText(this, "define escape", "scape string *[",
					QLineEdit::Normal, m_strEscape , &ok);
	if(ok)
	{
		m_strEscape = "";
		menuBar()->setItemChecked(ID_EDIT_ESC_NO,false);
		menuBar()->setItemChecked(ID_EDIT_ESC_ESC,false);
		menuBar()->setItemChecked(ID_EDIT_ESC_U,false);
		menuBar()->setItemChecked(ID_EDIT_ESC_CUS,false);

		m_strEscape = strEsc;
		menuBar()->setItemChecked(ID_EDIT_ESC_CUS,true);
	}
}

void QTermFrame::gbkCodec()
{
	menuBar()->setItemChecked(ID_EDIT_CODEC_GBK,false);
	menuBar()->setItemChecked(ID_EDIT_CODEC_BIG5,false);

	m_nClipCodec=0;
	menuBar()->setItemChecked(ID_EDIT_CODEC_GBK,true);
}

void QTermFrame::big5Codec()
{
	menuBar()->setItemChecked(ID_EDIT_CODEC_GBK,false);
	menuBar()->setItemChecked(ID_EDIT_CODEC_BIG5,false);

	m_nClipCodec=1;
	menuBar()->setItemChecked(ID_EDIT_CODEC_BIG5,true);
}

void QTermFrame::font( )
{
	wndmgr->activeWindow()->font();
}

void QTermFrame::color( )
{
	wndmgr->activeWindow()->color();

}
void QTermFrame::refresh( )
{
	wndmgr->activeWindow()->refresh();
}

void QTermFrame::uiFont()
{
	bool ok;
	QFont font = QFontDialog::getFont(&ok,qApp->font());
	
	if(m_pref.bAA)
		font.setStyleStrategy(QFont::PreferAntialias);
	
	if(ok==true)
	{
		qApp->setFont(font,TRUE);
	}
}

void QTermFrame::fullscreen()
{
	m_bFullScreen = ! m_bFullScreen;
	
	if( m_bFullScreen )
	{
		menuBar()->hide();
		mdiTools->hide();
		mdiconnectTools->hide();
		key->hide();
		hideScroll();
		showStatusBar();
		showSwitchBar();
		showFullScreen();
	}
	else
	{
		menuBar()->show();
		mdiTools->show();
		mdiconnectTools->show();
		key->show();
		emit updateScroll();
		showStatusBar();
		showSwitchBar();
		showNormal();
	}

	menuBar()->setItemChecked( ID_VIEW_FULL, m_bFullScreen );

}

void QTermFrame::bosscolor()
{
	m_bBossColor = !m_bBossColor;

	emit bossColor();
	
	menuBar()->setItemChecked( ID_VIEW_BOSS, m_bBossColor );
}

void QTermFrame::themesMenuAboutToShow()
{
	themesMenu->clear();
// #if QT_VERSION < 300
// 	insertThemeItem("default");	
// 	#ifndef QT_NO_STYLE_CDE
// 	insertThemeItem("CDE");
// 	#endif
// 	#ifndef QT_NO_STYLE_MOTIF
// 	insertThemeItem("Motif");
// 	#endif
// 	#ifndef QT_NO_STYLE_MOTIFPLUS
// 	insertThemeItem("MotifPlus");
// 	#endif
// 	#ifndef QT_NO_STYLE_PLATINUM
// 	insertThemeItem("Platinum");
// 	#endif
// 	#ifndef QT_NO_STYLE_SGI
// 	insertThemeItem("SGI");
// 	#endif
// 	#ifndef QT_NO_STYLE_WINDOWS
// 	insertThemeItem("Windows");
// 	#endif
// #else
	QStringList styles = QStyleFactory::keys();
	for(QStringList::ConstIterator it=styles.begin(); it!=styles.end(); it++)
		insertThemeItem(*it);
// #endif
}

void QTermFrame::themesMenuActivated( int id )
{

	theme = themesMenu->text(id);
// 	fprintf(stderr, "Theme: %s\n", theme.ascii());

// #if QT_VERSION < 300
// 	if(theme=="default")
// 	{
// 	}
// 	else if(theme=="CDE")
// 	{
// 		#ifndef QT_NO_STYLE_CDE
// 		qApp->setStyle( new QCDEStyle(true) );
// 		#endif
// 	}
// 	else if(theme=="Motif")
// 	{
// 		#ifndef QT_NO_STYLE_MOTIF
// 		qApp->setStyle( new QMotifStyle(true) );
// 		#endif
// 	}
// 	else if(theme=="MotifPlus")
// 	{
// 		#ifndef QT_NO_STYLE_MOTIFPLUS
// 		qApp->setStyle( new QMotifPlusStyle(true) );
// 		#endif
// 	}
// 	else if(theme=="Platinum")
// 	{
// 		#ifndef QT_NO_STYLE_PLATINUM
// 		qApp->setStyle( new QPlatinumStyle() );
// 		#endif
// 	}
// 	else if(theme=="SGI")
// 	{
// 		#ifndef QT_NO_STYLE_SGI
// 		qApp->setStyle( new QSGIStyle(true) );
// 		#endif
// 	}
// 	else if(theme=="Windows")
// 	{
// 		#ifndef QT_NO_STYLE_WINDOWS
// 		qApp->setStyle( new QWindowsStyle() );
// 		#endif
// 	}
// #else
	QStyle * style = QStyleFactory::create(theme);
	if (style)
		qApp->setStyle(style);
// #endif
	
}

void QTermFrame::hideScroll()
{
	menuBar()->setItemChecked( ID_VIEW_SCROLL_RIGHT, false );
	menuBar()->setItemChecked( ID_VIEW_SCROLL_LEFT, false );
	menuBar()->setItemChecked( ID_VIEW_SCROLL_HIDE, true );

	m_nScrollPos = 0;

	emit updateScroll();
}
void QTermFrame::leftScroll()
{
	menuBar()->setItemChecked( ID_VIEW_SCROLL_HIDE, false );
	menuBar()->setItemChecked( ID_VIEW_SCROLL_RIGHT, false );
	menuBar()->setItemChecked( ID_VIEW_SCROLL_LEFT, true );
	
	m_nScrollPos = 1;

	emit updateScroll();
}
void QTermFrame::rightScroll()
{
	menuBar()->setItemChecked( ID_VIEW_SCROLL_HIDE, false );
	menuBar()->setItemChecked( ID_VIEW_SCROLL_LEFT, false );
	menuBar()->setItemChecked( ID_VIEW_SCROLL_RIGHT, true );
	
	m_nScrollPos = 2;

	emit updateScroll();
}

void QTermFrame::showSwitchBar()
{

	m_bSwitchBar = !m_bSwitchBar; 
	menuBar()->setItemChecked(ID_VIEW_SWITCH,m_bSwitchBar );
	if(m_bSwitchBar)
		statusBar()->show();
	else
		statusBar()->hide();
}

void QTermFrame::showStatusBar()
{
	m_bStatusBar = !m_bStatusBar;
	menuBar()->setItemChecked(ID_VIEW_STATUS,m_bStatusBar );

	emit updateStatusBar(m_bStatusBar);
}

void QTermFrame::setting( )
{
	wndmgr->activeWindow()->setting();
}
void QTermFrame::defaultSetting()
{
	addrDialog set(this, true);
	
	QTermConfig *pConf = new QTermConfig(addrCfg);

	if( pConf->hasSection("default") )
		loadAddress(pConf,-1,set.param);

	set.updateData(false);

	if(set.exec()==1)
	{
		saveAddress(pConf,-1,set.param);
		pConf->save(addrCfg);
	}
	
	delete pConf;
}

void QTermFrame::preference()
{
	prefDialog pref(this);

	if(pref.exec()==1)
	{
		QTermConfig *pConf = new QTermConfig(fileCfg);
		loadPref(pConf);
		delete pConf;
		setUseDock(m_pref.bTray);
	}
}

void QTermFrame::keySetup()
{
	keyDialog keyDlg(this);
	if(keyDlg.exec()==1)
	{
		updateKeyToolBar();
	}
}


void QTermFrame::antiIdle( )
{
	wndmgr->activeWindow()->antiIdle();

	menuBar()->setItemChecked( ID_SPEC_ANTI, wndmgr->activeWindow()->m_bAntiIdle );

	specAnti->setOn( wndmgr->activeWindow()->m_bAntiIdle );
}

void QTermFrame::autoReply( )
{
	wndmgr->activeWindow()->autoReply();

	menuBar()->setItemChecked( ID_SPEC_AUTO, wndmgr->activeWindow()->m_bAutoReply );

	specAuto->setOn( wndmgr->activeWindow()->m_bAutoReply );

}
void QTermFrame::viewMessages( )
{
	wndmgr->activeWindow()->viewMessages();
}
void QTermFrame::enableMouse( )
{
	wndmgr->activeWindow()->m_bMouse = !wndmgr->activeWindow()->m_bMouse;

	menuBar()->setItemChecked( ID_SPEC_MOUSE, wndmgr->activeWindow()->m_bMouse );

	specMouse->setOn( wndmgr->activeWindow()->m_bMouse );

}

void QTermFrame::viewImages()
{
	QTermImage viewer(pathPic+"pic/shadow.png", m_pref.strPoolPath, this);
	viewer.exec();
}

void QTermFrame::beep()
{
	wndmgr->activeWindow()->m_bBeep = !wndmgr->activeWindow()->m_bBeep;

	menuBar()->setItemChecked( ID_SPEC_BEEP, wndmgr->activeWindow()->m_bBeep );

	specBeep->setOn( wndmgr->activeWindow()->m_bBeep );

}

void QTermFrame::reconnect()
{
	wndmgr->activeWindow()->m_bReconnect = !wndmgr->activeWindow()->m_bReconnect;

	specReconnect->setOn( wndmgr->activeWindow()->m_bReconnect );
}

void QTermFrame::runScript()
{
	wndmgr->activeWindow()->runScript();
}
void QTermFrame::stopScript()
{
	wndmgr->activeWindow()->stopScript();
}

void QTermFrame::keyClicked(int id)
{
	if(wndmgr->activeWindow()==NULL)
		return;

	QTermConfig conf(fileCfg);

	QString strItem = QString("key%1").arg(id);
// 	strItem.sprintf("key%d",id);
	QString strTmp = conf.getItemValue("key", strItem);

	if(strTmp[0]=='0')	// key
	{
		wndmgr->activeWindow()->externInput(strTmp.mid(1).toLatin1());
	}else if(strTmp[0]=='1')	// script
	{
		wndmgr->activeWindow()->runScriptFile(strTmp.mid(1).toLatin1());
	}
	else if(strTmp[0]=='2')	// program
	{
		system((strTmp.mid(1)+" &").toLocal8Bit());
	}
}

void QTermFrame::toolBarPosChanged(QToolBar*)
{
	QTermConfig conf(fileCfg);
	
	Qt::ToolBarDock dock;
	int index;
	bool nl;
	int extra;
	
//	if(bar==mdiTools)
	{
// 		getLocation(mdiTools, dock, index, nl, extra);
// 		conf.setItemValue("global","mainbar",valueToString(mdiTools->isVisible(), (int)dock, index, nl, extra));
	}
//	else if(bar==key)
	{
// 		getLocation(key, dock, index, nl, extra);
// 		conf.setItemValue("global","keybar",valueToString(key->isVisible(), (int)dock, index, nl, extra));
	}
//	else if(bar==mdiconnectTools)
	{
// 		getLocation(mdiconnectTools, dock, index, nl, extra);
// 		conf.setItemValue("global","bbsbar", valueToString(mdiconnectTools->isVisible(), (int)dock, index, nl, extra));
	}

	conf.save(fileCfg);
}

void QTermFrame::addMainTool()
{
	
	// hte main toolbar	
	mdiTools = addToolBar( "Main ToolBar" );
// 	mdiTools->setLabel("Main ToolBar");

	QTermConfig conf(fileCfg);
	int hide,dock,index,nl,extra;

	QString strTmp = conf.getItemValue("global","mainbar");
	if(!strTmp.isEmpty())
	{
		sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
// 		moveToolBar( mdiTools, Qt::ToolBarDock(dock), nl==1, index, extra );
		if(hide==0)
			mdiTools->hide();
	}
	connectButton = new QToolButton( QPixmap(pathPic+"pic/connect.png"), tr("Connect"), 
			QString::null, NULL, NULL, mdiTools, "Connect" );
 	mdiTools->addWidget(connectButton);
	connectMenu = new QMenu(this);
	connect( connectMenu, SIGNAL(aboutToHide()), this, SLOT(connectMenuAboutToHide()) );
	connect( connectButton, SIGNAL(pressed()), this, SLOT(popupConnectMenu()) );
	
	mdiTools->addWidget(new QToolButton( QPixmap(pathPic+"pic/quick.png"), tr("Quick Login"), 
						QString::null, this, SLOT(quickLogin()), mdiTools, "Quick Login" ));
	// custom define
	key = addToolBar("Custom Key");
// 	key->setLabel(tr("Custom Key"));
	strTmp = conf.getItemValue("global","keybar");
	if(!strTmp.isEmpty())
	{
		sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
// 		moveToolBar( key, Qt::ToolBarDock(dock), nl==1, index, extra );
		if(hide==0)
			key->hide();
	}

	// the toolbar 
	mdiconnectTools = addToolBar( "bbs operations" );
	strTmp = conf.getItemValue("global","bbbar");
	if(!strTmp.isEmpty())
	{
		sscanf(strTmp.toLatin1(),"%d %d %d %d %d",&hide,&dock,&index,&nl,&extra);
// 		moveToolBar( mdiconnectTools, Qt::ToolBarDock(dock), nl==1, index, extra );
	}

	disconnectButton =
		new QToolButton( QPixmap(pathPic+"pic/disconnect.png"), tr("Disconnect"), QString::null,
			this, SLOT(disconnect()), mdiconnectTools, "Disconnect" );
	mdiconnectTools->addWidget(disconnectButton);
	disconnectButton->setEnabled(FALSE);
	mdiconnectTools->addSeparator();

	// Edit (5)
	mdiconnectTools->addWidget(new QToolButton( QPixmap(pathPic+"pic/copy.png"), tr("Copy"), QString::null,
							   this, SLOT(copy()), mdiconnectTools, "Copy" ));
	mdiconnectTools->addWidget(new QToolButton( QPixmap(pathPic+"pic/paste.png"), tr("Paste"), QString::null,
							   this, SLOT(paste()), mdiconnectTools, "Paste" ));
	editRect	=
		new QToolButton( QPixmap(pathPic+"pic/rect.png"), tr("Rectangle Select"), QString::null,
			this, SLOT(copyRect()), mdiconnectTools, "Rectangle Select" );
	editRect->setToggleButton(TRUE);
	mdiconnectTools->addWidget(editRect);
	editColor	=
		new QToolButton( QPixmap(pathPic+"pic/color-copy.png"), tr("Copy With Color"), QString::null,
			this, SLOT(copyColor()), mdiconnectTools, "Copy With Color" );
	editColor->setToggleButton(TRUE);
	mdiconnectTools->addWidget(editColor);
	mdiconnectTools->addSeparator();

	//View (3)
	mdiconnectTools->addWidget(new QToolButton( QPixmap(pathPic+"pic/fonts.png"), tr("Font"), QString::null,
							   this, SLOT(font()),  mdiconnectTools, "Font" ));
	mdiconnectTools->addWidget(new QToolButton( QPixmap(pathPic+"pic/color.png"), tr("Color"), QString::null,
							   this, SLOT(color()), mdiconnectTools, "Color" ));
	mdiconnectTools->addWidget(new QToolButton( QPixmap(pathPic+"pic/refresh.png"), tr("Refresh Screen"), QString::null,
							   this, SLOT(refresh()), mdiconnectTools, "Refresh Screen" ));
	mdiconnectTools->addSeparator();
	
	// Option 
	mdiconnectTools->addWidget(new QToolButton( QPixmap(pathPic+"pic/pref.png"), tr("Advanced Option"), QString::null,
							   this, SLOT(setting()), mdiconnectTools, "Advanced Option" ));
	mdiconnectTools->addSeparator();

	// Spec (5)
	mdiconnectTools->addWidget(new QToolButton( QPixmap(pathPic+"pic/article.png"), tr("Copy Article"), QString::null,
							   this, SLOT(copyArticle()), mdiconnectTools, "Copy Article" ));
	specAnti =
		new QToolButton( QPixmap(pathPic+"pic/anti-idle.png"), tr("Anti-Idle"), QString::null,
			this, SLOT(antiIdle()), mdiconnectTools, "Anti-Idle" );
	specAnti->setToggleButton(TRUE);
	mdiconnectTools->addWidget(specAnti);
	specAuto =
		new QToolButton( QPixmap(pathPic+"pic/auto-reply.png"), tr("Auto Reply"), QString::null,
			this, SLOT(autoReply()), mdiconnectTools, "Auto Reply" );
	specAuto->setToggleButton(TRUE);
	mdiconnectTools->addWidget(specAuto);
	mdiconnectTools->addWidget(new QToolButton( QPixmap(pathPic+"pic/message.png"), tr("View Message"), QString::null,
							   this, SLOT(viewMessages()), mdiconnectTools, "View Message" ));
	specMouse	=
	     new QToolButton( QPixmap(pathPic+"pic/mouse.png"), tr("Enable Mouse Support"), QString::null,
			this, SLOT(enableMouse()), mdiconnectTools, "Enable Mouse Support" );
	specMouse->setToggleButton(TRUE);
	mdiconnectTools->addWidget(specMouse);
	specBeep =
		new QToolButton( QPixmap(pathPic+"pic/sound.png"), tr("Beep When Message Coming"), QString::null,
			this, SLOT(beep()), mdiconnectTools, "Beep When Message Coming" );
	specBeep->setToggleButton(TRUE);
	mdiconnectTools->addWidget(specBeep);
	specReconnect = 
		new QToolButton( QPixmap(pathPic+"pic/reconnect.png"), tr("Reconnect When Disconnected By Host"), QString::null,
			this, SLOT(reconnect()), mdiconnectTools, "Reconnect When Disconnected By Host" );
	specReconnect->setToggleButton(TRUE);
	mdiconnectTools->addWidget(specReconnect);
}


void QTermFrame::addMainMenu()
{
	mainMenu = menuBar();//new QMenuBar(this);
	
	QMenu * file = mainMenu->addMenu( tr("&File") );
// 	mainMenu->insertItem( tr("&File"), file );
	file->insertItem( QPixmap(pathLib+"pic/connect.png"), 
					tr("&Connect"), this, SLOT(connectIt()) );
	file->insertItem( QPixmap(pathLib+"pic/disconnect.png"),
					 tr("&Disconnect"), this, SLOT(disconnect()));//, 0, ID_FILE_DISCONNECT );

	file->addSeparator();
	file->insertItem( QPixmap(pathLib+"pic/addr.png"), 
					tr("&Address book"), this, SLOT(addressBook()), Qt::Key_F2 );
	file->insertItem( QPixmap(pathLib+"pic/quick.png"), 
					tr("&Quick login"),this, SLOT(quickLogin()), Qt::Key_F3 );
	file->addSeparator();
	file->insertItem( tr("&Exit"), this, SLOT(exitQTerm()) );
	
	//Edit Menu
	QMenu * edit = new QMenu( this );
	mainMenu->insertItem( tr("&Edit"), edit );

	edit->setCheckable( true );
	edit->insertItem( QPixmap(pathLib+"pic/copy.png"),
					tr("&Copy"), this, SLOT(copy()), Qt::CTRL+Qt::Key_Insert, ID_EDIT_COPY );
	edit->insertItem( QPixmap(pathLib+"pic/paste.png"),
					tr("&Paste"), this, SLOT(paste()), Qt::SHIFT+Qt::Key_Insert, ID_EDIT_PASTE );
	edit->insertSeparator();
	edit->insertItem( QPixmap(pathLib+"pic/color-copy.png"),
					tr("C&opy with color"), this, SLOT(copyColor()), 0, ID_EDIT_COLOR );
	edit->insertItem( QPixmap(pathLib+"pic/rect.png"),
					tr("&Rectangle select"), this, SLOT(copyRect()), 0, ID_EDIT_RECT );
	edit->insertItem( tr("Auto copy &select"), this, SLOT(autoCopy()),0,  ID_EDIT_AUTO );
	edit->insertItem( tr("P&aste with wordwrap"), this, SLOT(wordWrap()), 0,  ID_EDIT_WW );
	
	QMenu * escapeMenu = new QMenu(this);
	escapeMenu->setCheckable(true);
		escapeMenu->insertItem( tr("&None"), this, SLOT(noEsc()), 0, ID_EDIT_ESC_NO );
		escapeMenu->insertItem( tr("&ESC ESC ["), this, SLOT(escEsc()), 0, ID_EDIT_ESC_ESC );
		escapeMenu->insertItem( tr("Ctrl+&U ["), this, SLOT(uEsc()), 0, ID_EDIT_ESC_U );
		escapeMenu->insertItem( tr("&Custom..."), this, SLOT(customEsc()), 0, ID_EDIT_ESC_CUS );
	edit->insertItem( tr("Paste &with color"), escapeMenu);

	QMenu * codecMenu = new QMenu(this);
	codecMenu->setCheckable(true);
		codecMenu->insertItem( tr("&GBK"), this, SLOT(gbkCodec()), 0, ID_EDIT_CODEC_GBK );
		codecMenu->insertItem( tr("&Big5"), this, SLOT(big5Codec()), 0, ID_EDIT_CODEC_BIG5 );
	edit->insertItem( tr("Clipboard &encoding"), codecMenu);

	//View menu
	QMenu * view = new QMenu( this );
	mainMenu->insertItem( tr("&View"), view );
	
	view->setCheckable( true );
	view->insertItem( QPixmap(pathLib+"pic/fonts.png"),
					tr("&Font"), this, SLOT(font()), 0, ID_VIEW_FONT );
	view->insertItem( QPixmap(pathLib+"pic/color.png"),
					tr("&Color"), this, SLOT(color()), 0, ID_VIEW_COLOR );
	view->insertItem( QPixmap(pathLib+"pic/refresh.png"),
					tr("&Refresh"), this, SLOT(refresh()), Qt::Key_F5, ID_VIEW_REFRESH );
	view->insertSeparator();
	//language menu
	langMenu = new QMenu( this );
	langMenu->setCheckable(true);
		sEng = langMenu->insertItem( tr("&English"),this,SLOT(langEnglish()));
		sChs = langMenu->insertItem( tr("&Simplified Chinese"),this,SLOT(langSimplified()));
		sCht = langMenu->insertItem( tr("&Traditional Chinese"),this,SLOT(langTraditional()));
	view->insertItem( tr("&Language"), langMenu );
	view->insertItem( tr("&UI font"), this, SLOT(uiFont()) );

	themesMenu = new QMenu( this );
	themesMenu->setCheckable( TRUE );
	connect( themesMenu, SIGNAL( aboutToShow() ),
	     this, SLOT( themesMenuAboutToShow() ) );
	view->insertItem( tr("&Themes"), themesMenu );

// 	Q3Accel *a = new Q3Accel(this);
// 	a->connectItem(a->insertItem(Qt::Key_F6), this, SLOT(fullscreen()) );
	view->insertItem( tr("&Fullscreen"), this, SLOT(fullscreen()), Qt::Key_F6, ID_VIEW_FULL );

	view->insertItem( tr("Boss &Color"), this, SLOT(bosscolor()), Qt::Key_F12, ID_VIEW_BOSS );

	view->insertSeparator();
	QMenu *scrollMenu = new QMenu(this);
		scrollMenu->insertItem( tr("&Hide"), this, SLOT(hideScroll()), 0, ID_VIEW_SCROLL_HIDE );
		scrollMenu->insertItem( tr("&Left"), this, SLOT(leftScroll()), 0, ID_VIEW_SCROLL_LEFT );
		scrollMenu->insertItem( tr("&Right"), this, SLOT(rightScroll()), 0, ID_VIEW_SCROLL_RIGHT );
	view->insertItem( tr("&ScrollBar"), scrollMenu );
	view->insertItem( tr("Status &Bar"), this, SLOT(showStatusBar()), 0, ID_VIEW_STATUS );
	view->insertItem( tr("S&witch Bar"), this, SLOT(showSwitchBar()), 0, ID_VIEW_SWITCH );

	
	// Option Menu	
	QMenu * option = new QMenu( this );
	mainMenu->insertItem( tr("&Option"), option );

	option->insertItem( tr("&Setting for currrent session"), this, SLOT(setting()), 0, ID_OPTION_CURRENT );
	option->insertSeparator();
	option->insertItem( tr("&Default setting"), this, SLOT(defaultSetting()) );
	option->insertItem( tr("&Preference"), this, SLOT(preference()) );
	
	// Special
	QMenu * spec = new QMenu( this );
	mainMenu->insertItem( tr("&Special"), spec );
	spec->insertItem( QPixmap(pathLib+"pic/article.png"),
					tr("&Copy article"), this, SLOT(copyArticle()), Qt::Key_F9, ID_SPEC_ARTICLE );
	spec->setCheckable( true );
	spec->insertItem( QPixmap(pathLib+"pic/anti-idle.png"),
					tr("Anti &idle"), this, SLOT(antiIdle()), 0, ID_SPEC_ANTI );
	spec->insertItem(QPixmap(pathLib+"pic/auto-reply.png"), 
					tr("Auto &reply"), this, SLOT(autoReply()), 0, ID_SPEC_AUTO );
	spec->insertItem( QPixmap(pathLib+"pic/message.png"),
					tr("&View messages"), this, SLOT(viewMessages()), Qt::Key_F10, ID_SPEC_MESSAGE );
	spec->insertItem( QPixmap(pathLib+"pic/sound.png"),
					tr("&Beep "), this, SLOT(beep()), 0, ID_SPEC_BEEP );
	spec->insertItem( QPixmap(pathLib+"pic/mouse.png"),
					tr("&Mouse support"), this, SLOT(enableMouse()), 0, ID_SPEC_MOUSE );
	spec->insertItem(tr("&Image viewer"), this, SLOT(viewImages()));

	
	//Script
	QMenu * script = new QMenu( this );
	mainMenu->insertItem( tr("Scrip&t"), script );
	script->insertItem( tr("&Run..."), this, SLOT(runScript()), Qt::Key_F7, ID_SCRIPT_RUN );
	script->insertItem( tr("&Stop"), this, SLOT(stopScript()), Qt::Key_F8, ID_SCRIPT_STOP );

	//Window menu
	windowsMenu = new QMenu( this );
	windowsMenu->setCheckable( TRUE );
	connect( windowsMenu, SIGNAL( aboutToShow() ),
	     this, SLOT( windowsMenuAboutToShow() ) );

	mainMenu->insertItem( tr("&Windows"), windowsMenu );
	mainMenu->insertSeparator();

	//Help menu
	QMenu * help = new QMenu( this );
	mainMenu->insertItem( tr("&Help"), help );
	help->insertItem( tr("About &QTerm"), this, SLOT(aboutQTerm()), Qt::Key_F1 );
	help->insertItem( tr("QTerm's &Homepage"),this,SLOT(homepage()));

}

void QTermFrame::updateMenuToolBar()
{
	QTermWindow * window = wndmgr->activeWindow();

	if(window==NULL)
		return;

	// update menu
	mainMenu->setItemEnabled( ID_FILE_DISCONNECT, window->isConnected() );
	
	mainMenu->setItemChecked( ID_EDIT_COLOR, window->m_bCopyColor );
	mainMenu->setItemChecked( ID_EDIT_RECT, window->m_bCopyRect );
	mainMenu->setItemChecked( ID_EDIT_AUTO,	window->m_bAutoCopy );
	mainMenu->setItemChecked( ID_EDIT_WW,	window->m_bWordWrap );

	mainMenu->setItemChecked( ID_VIEW_FULL, m_bFullScreen );


	mainMenu->setItemChecked( ID_SPEC_ANTI,	window->m_bAntiIdle );
	mainMenu->setItemChecked( ID_SPEC_AUTO,	window->m_bAutoReply );
	mainMenu->setItemChecked( ID_SPEC_BEEP,	window->m_bBeep );
	mainMenu->setItemChecked( ID_SPEC_MOUSE, window->m_bMouse );

	// update toolbar
	
	disconnectButton->setEnabled( window->isConnected() );

	editColor->setOn( window->m_bCopyColor );
	editRect->setOn( window->m_bCopyRect );

	specAnti->setOn( window->m_bAntiIdle );
	specAuto->setOn( window->m_bAutoReply );
	specBeep->setOn( window->m_bBeep );
	specMouse->setOn( window->m_bMouse );
	specReconnect->setOn( window->m_bReconnect );
}

void QTermFrame::enableMenuToolBar( bool enable )
{
	mainMenu->setItemEnabled( ID_FILE_DISCONNECT,	enable );
	
	mainMenu->setItemEnabled( ID_EDIT_COPY, 	enable );
	mainMenu->setItemEnabled( ID_EDIT_PASTE, 	enable );
	mainMenu->setItemEnabled( ID_EDIT_COLOR,	enable );
	mainMenu->setItemEnabled( ID_EDIT_RECT,		enable );
	mainMenu->setItemEnabled( ID_EDIT_AUTO, 	enable );
	mainMenu->setItemEnabled( ID_EDIT_WW, 		enable );
	
	mainMenu->setItemEnabled( ID_VIEW_FONT, 	enable );
	mainMenu->setItemEnabled( ID_VIEW_COLOR, 	enable );
	mainMenu->setItemEnabled( ID_VIEW_REFRESH, 	enable );

	mainMenu->setItemEnabled( ID_OPTION_CURRENT,enable );

	mainMenu->setItemEnabled( ID_SPEC_ARTICLE, 	enable );
	mainMenu->setItemEnabled( ID_SPEC_ANTI,		enable );
	mainMenu->setItemEnabled( ID_SPEC_AUTO,		enable );
	mainMenu->setItemEnabled( ID_SPEC_MESSAGE,	enable );
	mainMenu->setItemEnabled( ID_SPEC_BEEP,		enable );
	mainMenu->setItemEnabled( ID_SPEC_MOUSE,	enable );

	mainMenu->setItemEnabled( ID_SCRIPT_RUN,	enable );
	mainMenu->setItemEnabled( ID_SCRIPT_STOP,	enable );

	if( enable )
		mdiconnectTools->show();
	else
		mdiconnectTools->hide();

	return;
}

void QTermFrame::updateKeyToolBar()
{
	key->clear();
	new QToolButton( QPixmap(pathPic+"pic/keys.png"), tr("Key Setup"), QString::null,
			this, SLOT(keySetup()), key, "Key Setup" );

	QTermConfig * conf= new QTermConfig(fileCfg);
	QString strItem, strTmp;
	strTmp = conf->getItemValue("key", "num");
	int num = strTmp.toInt();

	for(int i=0; i<num; i++)
	{
		strItem = QString("name%1").arg(i);
		strTmp = QString::fromLocal8Bit(conf->getItemValue("key", strItem));
		QTermToolButton *button = new QTermToolButton(key, i, strTmp);
		button->setUsesTextLabel(true);
		button->setTextLabel(strTmp, false);
		button->setTextPosition(QToolButton::BesideIcon);
		strItem = QString("key%1").arg(i);
		strTmp = QString::fromLocal8Bit(conf->getItemValue("key", strItem));
// 		QToolTip::add( button, strTmp.mid(1) );
// 		button->addToolTip(strTmp.mid(1));
		connect(button, SIGNAL(buttonClicked(int)), this, SLOT(keyClicked(int)));
	}

	delete conf;
}

QString QTermFrame::valueToString(bool shown, int dock, int index, bool nl, int extra )
{
	QString str="";

	str = QString("%1 %2 %3 %4 %5").arg(shown?1:0).arg(dock).arg(index).arg(nl?1:0).arg(extra);

	return str;
}

void QTermFrame::popupFocusIn(QTermWindow *)
{
	// bring to font
	if(isHidden()) {
		show();
	}
	if(isMinimized()) {
		if(isMaximized())
			showMaximized();
		else
			showNormal();
	}
	raise();
	setActiveWindow();
}

void QTermFrame::insertThemeItem(QString themeitem)
{
	//const char *style[]={"Default", "CDE", "Motif", 
			//"Motif Plus", "Platinum", "SGI", "Windows"};
	int id;

	id = themesMenu->insertItem(themeitem, this, SLOT(themesMenuActivated(int)));
	themesMenu->setItemParameter( id, id );
	themesMenu->setItemChecked( id, themeitem==theme );
}

void QTermFrame::setUseDock(bool use)
{
	if(use == false) {
		if(tray) {
			delete tray;
			tray = 0;
			delete trayMenu;
			trayMenu = 0;
		}
		return;
	}

	if(tray)
		return;

	trayMenu = new QMenu;
	connect(trayMenu, SIGNAL(aboutToShow()), SLOT(buildTrayMenu()));

	
	tray = new MTray( pathLib+"pic/qterm_tray.png", "QTerm", trayMenu, this);
	connect(tray, SIGNAL(clicked(const QPoint &, int)), SLOT(trayClicked(const QPoint &, int)));
	connect(tray, SIGNAL(doubleClicked(const QPoint &)), SLOT(trayDoubleClicked()));
	connect(tray, SIGNAL(closed()), this, SLOT(exitQTerm()));

	tray->show();
}

void QTermFrame::buildTrayMenu()
{
	if(!trayMenu)
		return;
	trayMenu->clear();

	if(isHidden())
		trayMenu->insertItem(tr("Show"), this, SLOT(trayShow()));
	else
		trayMenu->insertItem(tr("Hide"), this, SLOT(trayHide()));
	trayMenu->insertSeparator();	
	trayMenu->insertItem(tr("About"), this, SLOT(aboutQTerm()));
	trayMenu->insertItem(tr("Exit"), this, SLOT(exitQTerm()));
}

void QTermFrame::trayClicked(const QPoint &, int)
{
	if(isHidden())
		trayShow();
	else
		trayHide();
}

void QTermFrame::trayDoubleClicked()
{
	if(isHidden())
		trayShow();
	else
		trayHide();
}

void QTermFrame::trayShow()
{
	show();
	// bring to font
	if(isHidden()) {
		show();
	}
	if(isMinimized()) {
		if(isMaximized())
			showMaximized();
		else
			#ifdef Q_OS_MACX
			showMaximized();
			#else
			showNormal();
			#endif
	}
	raise();
	setActiveWindow();
}

void QTermFrame::trayHide()
{
	hide();
}

void QTermFrame::buzz()
{
	int xp = x();
	int yp = y();
	QTime t;

	t.start();
	for ( int i = 32; i > 0; )
	{
		//QApplication::processEvents();
		qApp->processEvents();
		if ( t.elapsed() >= 1 )
	 	{
			int delta = i >> 2;
			int dir = i & 3;
			int dx = ((dir==1)||(dir==2)) ? delta : -delta;
			int dy = (dir<2) ? delta : -delta;
			move( xp+dx, yp+dy );
			t.restart();
			i--;
			}
		}
	move( xp, yp );
}

//----------------------------------------------------------------------------
// MTray
//----------------------------------------------------------------------------
MTray::MTray(const QString &icon, const QString &tip, QMenu *popup, QObject *parent)
:QObject(parent)
{
	ti = new TrayIcon(QPixmap(icon), tip, popup);
	connect(ti, SIGNAL(clicked(const QPoint &, int)), SIGNAL(clicked(const QPoint &, int)));
	connect(ti, SIGNAL(doubleClicked(const QPoint &)), SIGNAL(doubleClicked(const QPoint &)));
	connect(ti, SIGNAL(closed()), SIGNAL(closed()));
	ti->show();
}

MTray::~MTray()
{
	delete ti;
}

void MTray::setToolTip(const QString &str)
{
	ti->setToolTip(str);
}

void MTray::setImage(const QImage &i)
{
	ti->setIcon(makeIcon(i));
}

void MTray::show()
{
	ti->show();
}

void MTray::hide()
{
	ti->hide();
}

QPixmap MTray::makeIcon(const QImage &_in)
{
#ifdef Q_WS_X11
		// on X11, the KDE dock is 22x22.  let's make our icon "seem" bigger.
		QImage real(22,22,32);
		//QImage in = _in.convertToImage();
		QImage in = _in;
		in.detach();
		real.fill(0);
		real.setAlphaBuffer(true);

		// make sure it is no bigger than 16x16
		if(in.width() > 16 || in.height() > 16)
			in = in.smoothScale(16,16);

		int xo = (real.width() - in.width()) / 2;
		int yo = (real.height() - in.height()) / 2;

		int n, n2;

		// draw a dropshadow
		for(n2 = 0; n2 < in.height(); ++n2) {
			for(n = 0; n < in.width(); ++n) {
				if(qAlpha(in.pixel(n,n2))) {
					int x = n + xo + 2;
					int y = n2 + yo + 2;
					real.setPixel(x, y, qRgba(0x80,0x80,0x80,0xff));
				}
			}
		}
		// draw the image
		for(n2 = 0; n2 < in.height(); ++n2) {
			for(n = 0; n < in.width(); ++n) {
				if(qAlpha(in.pixel(n,n2))) {
					QRgb c = in.pixel(n, n2);
					real.setPixel(n + xo, n2 + yo, qRgba(qRed(c), qGreen(c), qBlue(c), 0xff));
				}
			}
		}
		// create the alpha layer
		for(n2 = real.height()-2; n2 >= 0; --n2) {
			for(n = 0; n < real.width(); ++n) {
				uint c = real.pixel(n, n2);
				if(c > 0) {
					QRgb old = real.pixel(n, n2);
					real.setPixel(n, n2, qRgba(qRed(old), qGreen(old), qBlue(old), 0xff));
				}
			}
		}

		QPixmap icon;
		icon.convertFromImage(real);
		return icon;
	//}
#else
	QPixmap icon;
	icon.convertFromImage(_in);
	return icon;
#endif
}


#include <qtermframe.moc>
