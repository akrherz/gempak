#include "nwx_cmn.h"
#include "nwx_gui.h"

#define MAX_MAPS	30	/* maximum maps can be displayed for
							map menu */

/*
 * Private functions
 */
#ifdef Linux
static void _quitNwxOK_cb ( Widget, XtPointer, XtPointer ) __attribute__((noreturn));
#else
static void _quitNwxOK_cb ( Widget, XtPointer, XtPointer );
#endif
void mapw_dataCb    ( Widget, XtPointer, XtPointer );
void mapw_exitCb    ( Widget, XtPointer, XtPointer );
void mapw_exposeCb  ( Widget, XtPointer, XmDrawingAreaCallbackStruct* );
void mapw_mapCb     ( Widget, long, XtPointer );
void mapw_autoCb    ( Widget, XtPointer, XtPointer );
void mapw_pickstnEh ( Widget, XtPointer, XEvent*, Boolean* );
void mapw_resizeCb  ( Widget, XtPointer, XtPointer );
void mapw_zoomCb    ( Widget, XtPointer, XtPointer );

Widget	 mapCanvW;
Widget	 autoPulldownMenuW;
mapbnd_t mapBnd;
Widget 	 autoMenuBtnW;

/************************************************************************
 * nwxg_mapw.c								*
 *									*
 * This module creates the map selection window and defines its 	*
 *	callbacks.							*
 *									*
 * CONTENTS:								*
 *	mapw_create()	creates the map selection window.		*
 *	mapw_pickstnEh()callback for the selection in drawing area.	*
 *	mapw_rgstr()	register the map selection window to GEMPAK	*
 *	mapw_exposeCb() callback for the map drawing area expose event. *
 *	mapw_resizeCb() callback for the map drawing area resize event. *
 *	mapw_dataCb()	callback for the data button on menubar.	*
 *	mapw_mapCb()	callback for the map menu on menubar.		*
 *	mapw_autoCb()	callback for the auto menu on menubar.		*
 *	mapw_zoomCb()	callback for the zoom button on menubar.	*
 *	mapw_exitCb()	callback for the exit button on menubar.	*
 *	_quitNwxOK_cb() internal callback used in mapw_exitCb().	*
 ***********************************************************************/

/*=====================================================================*/

Widget mapw_create ( Widget parent )
/************************************************************************
 * mapw_create								*
 *									*
 * This function creates the map window.				*
 *									*
 *  Widget mapw_create ( parent )					*
 *									*
 * Input parameters:							*
 *	parent		Widget		The top level widget		*
 *									*
 * Output parameters:							*
 *									*
 * Return parameters:							*
 *	mapw_create	Widget		The map window widget		*
 *									*
 **									*
 * Log: 								*
 * C. Lin/EAI		 8/95						*
 * G. Krueger/EAI	 9/97  NxmMCloseReset -> NxmMclose_menuReset	*
 * G. Krueger/EAI	10/97  Add NxmVers_showTitle			*
 * G. Krueger/EAI	11/97  Renamed NxmHelp functions		*
 * I. Durham/GSC	 5/98  changed call for underscore		*
 * T. Piper/SAIC	 2/02  freed mapmenu.label			*
 * T. Piper/SAIC	 7/02  reinstated the select by state option	*
 * M. Gamazaychikov/SAIC 8/02  changed the logic in 'state' mode so the *
 *                             station is id'ed by state and country id *
 * R. Tian/SAIC		11/03  Added Auto pulldown menu			*
 * T. Piper/SAIC	02/04	Removed check for Motif 1.1		*
 * H. Zeng/SAIC		07/04  re-designed to use FormWidgetClass	*
 * E. Safford/SAIC	12/07	use wnxm_NxmClose_menuReset		*
 ***********************************************************************/
{
Widget		form, menubar, button;
Pixel		background;
int		i, jj, nmap;
Arg		args[10];
Cardinal	argcnt;
Widget		autoMenu, autoItem;
XmString	autoItemLbl;
Arg		al[5];
int		ac;

static _NXMmenuItem  mapmenu[MAX_MAPS];

/*---------------------------------------------------------------------*/
/*
 *  Create a form widget as the container widget
 */
	form = XtVaCreateWidget("mapform", xmFormWidgetClass, parent, NULL);

	XtVaGetValues(form, XmNbackground, &background, NULL);

/*
 *  Create the menubar
 */
	argcnt = 0;
	XtSetArg(args[argcnt],XmNtopAttachment,   XmATTACH_FORM); argcnt++;
	XtSetArg(args[argcnt],XmNleftAttachment,  XmATTACH_FORM); argcnt++;
	XtSetArg(args[argcnt],XmNrightAttachment, XmATTACH_FORM); argcnt++;
	menubar = XmCreateMenuBar(form, "topmenubar", args, argcnt);
	XtVaSetValues(menubar, XmNbackground, background, NULL);

/*
 *  Create Data pulldown menu
 */
	button = XmCreateCascadeButton(menubar, "Data", NULL, 0);
	XtVaSetValues(button, XmNmnemonic, 'D', NULL);
	XtAddCallback(button, XmNactivateCallback, mapw_dataCb, NULL);
	XtManageChild(button);

/*
 *  Create Map pulldown menu
 *
 *  Set the items for the menu bar for the map areas.
 */
	nmap = nwxTable->nmap;
	for ( i = 0; i < nmap; i++ ) {
	    mapmenu[i].label = (char *)
			malloc(strlen(nwxTable->map_info[i].name)+1);
	    strcpy ( mapmenu[i].label, nwxTable->map_info[i].name );
	    mapmenu[i].class	    = &xmCascadeButtonGadgetClass;
	    mapmenu[i].mnemonic     = 0;
	    mapmenu[i].accelerator  = NULL;
	    mapmenu[i].accel_text   = NULL;
	    mapmenu[i].callback     = (XtCallbackProc)mapw_mapCb;
	    mapmenu[i].which_widget = (long)i;
	    mapmenu[i].subitems     = NULL;
	    mapmenu[i].sub_buttons  = NULL;
	}
	mapmenu[nmap].label	   = NULL;
	mapmenu[nmap].class	   = &xmCascadeButtonGadgetClass;
	mapmenu[nmap].mnemonic	   = 0;
	mapmenu[nmap].accelerator  = NULL;
	mapmenu[nmap].accel_text   = NULL;
	mapmenu[nmap].callback	   = NULL;
	mapmenu[nmap].which_widget = (long)nmap;
	mapmenu[nmap].subitems	   = NULL;
	mapmenu[nmap].sub_buttons  = NULL;

	wnxm_NxmMenuPulldownBuild(menubar, NULL, "Map", 'M', mapmenu);
	for ( jj = 0; jj < nmap; jj++ ) free(mapmenu[jj].label);

/*
 *  Create Auto-Update pulldown menu, and make the OFF checked
 *  by default (__autoUpdt = 1).
 */
	ac = 0;
	XtSetArg ( al[ac], XmNradioBehavior, True );
	ac++;
	autoMenu = XmCreatePulldownMenu ( menubar, "autoMenu",
	    				      al, ac );
	ac = 0; 
	autoItemLbl = XmStringCreateLocalized ( "Auto-Update ON" );
	XtSetArg ( al[ac], XmNlabelString, autoItemLbl );
	ac++;
	XtSetArg ( al[ac], XmNindicatorType, XmONE_OF_MANY );
	ac++;
	autoItem = XmCreateToggleButtonGadget ( autoMenu, 
	           NULL, al, ac );   
	XtAddCallback ( autoItem, XmNvalueChangedCallback, 
		        (XtCallbackProc)mapw_autoCb, (XtPointer)G_TRUE );
	XtManageChild ( autoItem );
	XmStringFree ( autoItemLbl );

	ac = 0; 
	autoItemLbl = XmStringCreateLocalized ( "Auto-Update OFF" );
	XtSetArg ( al[ac], XmNlabelString, autoItemLbl );
	ac++;
	XtSetArg ( al[ac], XmNindicatorType, XmONE_OF_MANY );
	ac++;
	XtSetArg ( al[ac], XmNset, True );
	ac++;
	autoItem = XmCreateToggleButtonGadget ( autoMenu, 
	           NULL, al, ac );   
	XtAddCallback ( autoItem, XmNvalueChangedCallback, 
		        (XtCallbackProc)mapw_autoCb, (XtPointer)G_FALSE );
	XtManageChild ( autoItem );
	XmStringFree ( autoItemLbl );

	ac = 0;
	XtSetArg ( al[ac], XmNsubMenuId, autoMenu );
	ac++;
	autoItemLbl = XmStringCreateLocalized ( "Auto" );
	XtSetArg ( al[ac], XmNlabelString, autoItemLbl );
	ac++;
	autoMenuBtnW = XmCreateCascadeButtonGadget ( menubar, NULL, al, ac );
	XtVaSetValues(autoMenuBtnW, XmNmnemonic, 'A', NULL);
	XtManageChild ( autoMenuBtnW );
	XmStringFree ( autoItemLbl );
	XtSetSensitive ( autoMenuBtnW, False );

/*
 *  Create Zoom pulldown menu
 */
	button = XmCreateCascadeButton(menubar, "Zoom", NULL, 0);
	XtVaSetValues(button, XmNmnemonic, 'Z', NULL);
	XtAddCallback(button, XmNactivateCallback, mapw_zoomCb, NULL);
	XtManageChild(button);

/*
 *  Create Exit button
 */
	button = XmCreateCascadeButton(menubar, "Exit", NULL, 0);
	XtVaSetValues(button, XmNmnemonic, 'E', NULL);
	XtAddCallback(button, XmNactivateCallback, mapw_exitCb, NULL);
	XtManageChild(button);

/*
 *  Create Help button
 */
	button = XmCreateCascadeButton(menubar, "Help", NULL, 0);
	XtVaSetValues(button, XmNmnemonic, 'H', NULL);
	XtAddCallback(button, XmNactivateCallback, 
		(XtCallbackProc)wnxm_NxmHelp_helpBtnCb, (XtPointer)1);
	XtManageChild(button);

/*
 *  Put the help cascade button at the right most side of
 *  the menu bar
 */
	if( (button = XtNameToWidget(menubar, "Help") ) )
		XtVaSetValues(menubar, XmNmenuHelpWidget, button, NULL);

	XtManageChild(menubar);

/*
 *  Create the map drawing area
 */
	mapCanvW = XtVaCreateManagedWidget( "mapcanv",
				xmDrawingAreaWidgetClass, form,
				XmNtopAttachment,	  XmATTACH_WIDGET,
				XmNtopWidget,		  menubar,
				XmNleftAttachment,	  XmATTACH_FORM,
				XmNrightAttachment,	  XmATTACH_FORM,
				XmNbottomAttachment,	  XmATTACH_FORM,
				XmNresizable,		  TRUE,
				NULL );

/*
 *  Add the expose and resize callbacks.
 */
	XtAddCallback( mapCanvW, XmNexposeCallback, (XtCallbackProc)mapw_exposeCb, NULL );
	XtAddCallback( mapCanvW, XmNresizeCallback, (XtCallbackProc)mapw_resizeCb, NULL );

/*
 *  Add the station/state selection event.
 */
	XtAddEventHandler( mapCanvW, ButtonPressMask,	FALSE,
			   (XtEventHandler)mapw_pickstnEh, NULL );

	XtManageChild( form );

/*
 *  Create the product selection popup
 */
	dslw_create(form);

	return( form );
}

/*=====================================================================*/
/* ARGSUSED */
void mapw_pickstnEh ( Widget wid, XtPointer clnt, XEvent *event, Boolean *cvt )
/************************************************************************
 * mapw_pickstnEh							*
 *									*
 * This function will select a station/stations based on the position	*
 * of left mouse button.						*
 *									*
 * mapw_pickstnEh ( wid, clnt, event )					*
 *									*
 * Input parameters:							*
 *	wid		Widget		The input widget		*
 *	clnt		XtPointer	The input data for the widget	*
 *	*event		XEvent		The registered event		*
 **									*
 * C. Lin/EAI		 6/95	modify from rband_start 		*
 * C. Lin/EAI		 9/95						*
 * L. Williams/EAI	 6/96	srch_fosdFndrep -> fosd_getdata 	*
 * D.W.Plummer/NCEP	 9/96	added "O" type processing		*
 * D.W.Plummer/NCEP	11/96	disable prev and next buttons for OBS	*
 * D. Kidwell/NCEP	 5/99	Replaced MAX_PLOT_STN with LLSTFL       *
 * T. Piper/SAIC	 6/02	Removed unused selct_by variable	*
 * T. Piper/SAIC	 7/02	Added display of wbox date/time		*
 * R. Tian/SAIC		 4/03	Added append text mode			*
 * R. Tian/SAIC		 5/03	enable prev/nxt only for replace mode	*
 * R. Tian/SAIC		 7/03	Added sort for search string         	*
 * T. Piper/SAIC	01/04	Added check for < -7 from fosd_getdata	*
 * E. Safford/SAIC	12/07	use txtw_setPrevNext(), rm local access *
 *				 to stnindex				*
 ***********************************************************************/
{
int		ier;
float		xloc, yloc;

/*---------------------------------------------------------------------*/
/*
 * Check if it is a left mouse button.
 */
    if  ( event->xbutton.button == 1 ) {

	xloc = (float)event->xbutton.x;
	yloc = (float)event->xbutton.y;

	mapw_pickStation( xloc, yloc, &ier );

	if ( ier >= G_NORMAL ) {
	    XtUnmanageChild(dataSelectW);
	}
	else {
	    return;
	}
    } 
}

/*=====================================================================*/
/* ARGSUSED */
void mapw_exposeCb ( Widget w, XtPointer clnt, 
				XmDrawingAreaCallbackStruct *cbs )
/************************************************************************
 * mapw_exposeCb							*
 *									*
 * This routine is the callback for expose window.			*
 *									*
 * mapw_exposeCb ( w, which, cb_data )					*
 *									*
 * Input parameters:							*
 *	w		Widget		The input widget		*
 *	clnt		XtPointer	The input data for the widget	*
 *	*cbs		XmDrawingAreaCallbackStruct X callback structure*
 **									*
 * S. Jacobs/NMC	 8/94						*
 * T. Piper/SAIC	04/05	Added check on count			*
 * E. Safford/SAIC	12/07	use wnxm_xxflsh wrapper			*
 ***********************************************************************/
{
int    raise, iret;
XEvent *event;

/*---------------------------------------------------------------------*/
    event = cbs->event;

/*
 * Flush the graphics buffer.
 *
 * ES NOTE:  
 *  	This application should not be calling xxflsh directly.
 *	When time permits, rewrite this.  I think a simple geplot
 *	would work ok for this purpose.
 */
    if ( event->xexpose.count == 0 ) {
	raise = G_TRUE;
	wnxm_xxflsh ( &raise, &iret );
    }
}

/*=====================================================================*/
/* ARGSUSED */
void mapw_resizeCb ( Widget w, XtPointer clnt, XtPointer cbs )
/************************************************************************
 * mapw_resizeCb							*
 *									*
 * This routine is the callback for resize window.			*
 *									*
 * mapw_resizeCb ( w, clnt, cbs )					*
 *									*
 * Input parameters:							*
 *	w		Widget		The input widget		*
 *	clnt		int		The input data for the widget	*
 *	cbs		XtPointer	The output data for the widget	*
 **									*
 * S. Jacobs/NMC	 8/94		add check for valflg		*
 * C. Lin/EAI		10/95						*
 ***********************************************************************/
{
int  iret;
/*---------------------------------------------------------------------*/
/*
 * Clear the screen and replot.
 */
	wgem_gclear( &iret );
	fosd_plot( 1 );
}

/*=====================================================================*/
/* ARGSUSED */
void mapw_exitCb ( Widget w, XtPointer clnt, XtPointer cbs )
/************************************************************************
 * mapw_exitCb								*
 *									*
 * This routine is the callback for the exit button.			*
 *									*
 * mapw_exitCb ( w, clnt, cbs )						*
 *									*
 * Input parameters:							*
 *	w		Widget		The input widget		*
 *	clnt		XtPointer	The input data for the widget	*
 *	cbs	 	XtPointer	The output data for the widget	*
 **									*
 * S. Jacobs/NMC	 8/94						*
 * C. Lin/EAI		10/95	restructure				*
 * G. Krueger/EAI	 9/97	NxmExitDialog->NxmExit_create		*
 * E. Safford/GSC	02/01	update param list on NxmExit_create	*
 * E. Safford/GSC	12/07	use wnxm_NxmExit_create()          	*
 ***********************************************************************/
{
char	message[50];

/*---------------------------------------------------------------------*/
/*
 * Set the exit message and display the exit dialog box.
 */
    strcpy( message, "OK to Exit from NWX?" );
    wnxm_NxmExit_create( w, "Exit Confirmation", message, 
				_quitNwxOK_cb, NULL);
}

/*=====================================================================*/
/* ARGSUSED */
static void _quitNwxOK_cb ( Widget w, XtPointer clnt, XtPointer cbs )
/************************************************************************
 * _quitNwxOK_cb							*
 *									*
 * This routine is the callback for OK button in ExitDialog.		*
 *									*
 * _quitNwxOK_cb ( w, clnt, cbs )					*
 *									*
 * Input parameters:							*
 *	w		Widget		The input widget		*
 *	clnt		XtPointer	The input data for the widget	*
 *	cbs		XtPointer	The output data for the widget	*
 **									*
 * S. Jacobs/NMC	 8/94						*
 ***********************************************************************/
{
    exit(0);
}

/*=====================================================================*/
/* ARGSUSED */
void mapw_mapCb ( Widget w, long which, XtPointer cbs )
/************************************************************************
 * mapw_mapCb								*
 *									*
 * This routine is the callback for the map area menu.			*
 *									*
 * mapw_mapCb ( w, which, cbs )						*
 *									*
 * Input parameters:							*
 *	w		Widget		The input widget		*
 *	which		long		The item number for the menu	*
 *									*
 * Output parameters:							*
 *	cb_data 	XtPointer	The output data for the widget	*
 **									*
 * S. Jacobs/NMC	 8/94						*
 * L. Williams/EAI	12/94		add check for valflg		*
 * C. Lin/EAI		10/95						*
 * L. Williams/EAI	05/96		Add call to mapw_rmselstn.	*
 * S. Jacobs/NCEP	 3/99		Use NO_ZOOM instead of G_FALSE	*
 * R. Tian/SAIC		 6/03		Remove call to mapw_rmselstn.	*
 ***********************************************************************/
{
    int		ier;
/*---------------------------------------------------------------------*/

    mapw_updateMap( (int)which, &ier );

/*
 * Set the global variable for the map menu selection.
    usrSelect.mapindx = (int)which;
 */

/*
 * Draw the map window.
    usrSelect.zoomflg = NO_ZOOM;
    fosd_plot( 1 );
 */
}

/*=====================================================================*/
/* ARGSUSED */
void mapw_autoCb ( Widget w, XtPointer clnt, XtPointer call )
/************************************************************************
 * mapw_autoCb								*
 *									*
 * This routine is the callback for the auto menu.			*
 *									*
 * mapw_autoCb ( w, clnt, call )					*
 *									*
 * Input parameters:							*
 *	w		Widget		The input widget		*
 *	clnt		XtPointer	The item number for the menu	*
 *									*
 * Output parameters:							*
 *	call		XtPointer	The output data for the widget	*
 **									*
 * R. Tian/SAIC		11/03						*	
 ***********************************************************************/
{
    int which, ier;
    G_Boolean 	autoUpdt = G_FALSE;
/*---------------------------------------------------------------------*/
/*
 * This mapw_autoCb() routine is called twice each time a choice is
 * made from the Auto menu: First for the toggle being unset, then
 * for the toggle being set. Therefore, a response is needed only
 * if the toggle is set and it isn't already the current state. 
 */
    which = (long)clnt;

    autoUpdt = idata_getAutoUpdt( );

    if ( XmToggleButtonGadgetGetState ( w ) && which != autoUpdt ) {

	idata_setAutoUpdt( which, &ier );

        switch ( autoUpdt ) {
	    case 0:		/* ON */
                auto_startAutoUpdt();
            break;

	    case 1:		/* OFF */
                auto_stopAutoUpdt();
	    break;
        }
    }
}

/*=====================================================================*/
/* ARGSUSED */
void mapw_dataCb ( Widget w, XtPointer clnt, XtPointer call )
/************************************************************************
 * mapw_dataCb								*
 *									*
 * This function will pop up the data selection shell widget.		*
 *									*
 * void mapw_dataCb(w, clnt, call)					*
 *									*
 * Input Parameters:							*
 *  w			Widget						*
 *  clnt	 	XtPointer					*
 *  call		XtPointer					*
 *									*
 **									*
 * Log: 								*
 * L. Williams		06/95						*
 * H. Zeng/SAIC		07/04	made enhancements			*
 ***********************************************************************/
{
    if ( XtIsManaged(dataSelectW) ) {

	XtUnmanageChild (dataSelectW);
	XtManageChild(dataSelectW);
    }
    else {

	XtManageChild(dataSelectW);
    }
}

/*=====================================================================*/

int mapw_rgstr ( Widget mapwin )
/************************************************************************
 * mapw_rgstr								*
 *									*
 * This routine will initialize the GEMPAK display variables and	*
 * register the map drawing area into a GEMPAK window.			*
 *									*
 * int mapw_rgstr ( mapwin )						*
 *									*
 * Input parameters:							*
 *	mapwin	      	Widget		The map drawing area widget	*
 *									*
 * Return code: 							*
 *	mapw_rgstr	int		0 -- Successful.		*
 *					-1 -- Error.			*
 **									*
 * Log: 								*
 * S. Jacobs/NMC	 7/94						*
 * C. Lin/EAI		 3/95 use new color bank structure		*
 * C. Lin/EAI		 9/95						*
 * D.W.Plummer/NCEP	10/98   change fill rule to 'EvenOddRule'      	*
 * H. Zeng/EAI          04/00 changed cursor change function            *
 * T. Piper/SAIC	07/03	replaced gemdisplay with XtDisplay()	*
 * T. Piper/SAIC	07/03	replaced mapinit with gg_motf		*
 * T. Piper/SAIC	07/03	replaced routine with NxmGmpkRgstr	*
 ***********************************************************************/
{
int	iret;
char	wname[7]="nwxmap";
/*---------------------------------------------------------------------*/

	wnxm_NxmGmpkRgstr(mapwin, wname, NULL);

/*
 * Draw the US map.
 */

	draw_map(usrSelect.mapindx, nwxTable->map_info,
			(int)usrSelect.zoomflg, &mapBnd, &iret);

	return( 0 );
}

/*=====================================================================*/
/* ARGSUSED */
void mapw_zoomCb ( Widget w, XtPointer clnt, XtPointer cbs )
/************************************************************************
 * mapw_zoomCb								*
 *									*
 * This routine is the callback for the zoom button in the top menubar. *
 *									*
 * mapw_zoomCb ( w, clnt, cbs )						*
 *									*
 * Input parameters:							*
 *	w		Widget		The input widget		*
 *	clnt		XtPointer	The input data for the widget	*
 *	cbs	 	XtPointer	The callback data for the widget*
 **									*
 * C. Lin/EAI		 6/95		based on the obsolete function	*
 *					rband_end().			*
 * C. Lin/EAI		10/95						*
 * L. Williams/EAI	05/96		Add call to mapw_rmselstn.	*
 * S. Jacobs/NCEP	 6/98	Removed np from call to GGTPNT		*
 * S. Jacobs/NCEP	 3/99	Use ZOOM instead of G_TRUE		*
 * A. Hardy/GSC         11/00   renamed coordinate system declarations	*
 * T. Lee/GSC		01/01	Changed ityp to 13			*
 * T. Piper/SAIC	05/03	replaced XAllocNamedColor w/xsncolr	*
 * R. Tian/SAIC          6/03           Remove call to mapw_rmselstn.   *
 * T. Piper/SAIC	07/03	replaced gemdisplay with XtDisplay()	*
 * E. Safford/SAIC	12/07	use wgem_ggtpnt(), wgem_gtrans()	*
 ***********************************************************************/
{

static int    org_flg = 0;
static Pixel  org_zoombc, zoombc;
static int zflag = G_FALSE; /* flag for the zoom toggle button */

int    ii, ityp, np, iret;

float  xpts[2], ypts[2], xdev[2], ydev[2];

/*---------------------------------------------------------------------*/
/*
 * Toggle the Zoom Button
 */
	if (zflag == G_TRUE) {	/* zoom off */

	zflag = G_FALSE;

/*
 * Restore the button color
 */
	XtVaSetValues(w, XmNforeground, org_zoombc, NULL);

/*
 * Restore the cursor
 */
        wnxm_NxmCursor_setCursor(mapCanvW, CURS_DEFAULT);

/*
 * Restore mouse to be selection status
 */
	XSelectInput( XtDisplay(mapCanvW), XtWindow(mapCanvW),
			ButtonPressMask | ButtonReleaseMask |
			ExposureMask );

	XtAddEventHandler( mapCanvW, ButtonPressMask, FALSE,
			(XtEventHandler)mapw_pickstnEh, NULL );
	}
	else { /* zoom on */

	zflag = G_TRUE;

/*
 * Change mouse event handling for zooming
 */
	XtRemoveEventHandler( mapCanvW, ButtonPressMask, FALSE,
				(XtEventHandler)mapw_pickstnEh, NULL);

	usrSelect.zoomflg = ZOOM;

/*
 * get the original zoom button color and
 * the color in zoom status when needed.
 */
	if ( !org_flg ) {

	    XtVaGetValues(w, XmNforeground, &org_zoombc, NULL);
	    xsncolr("red", &zoombc, &iret);
	    org_flg = 1;
	}

/*
 * change to the zoom cursor
 */
	wnxm_NxmCursor_setCursor(mapCanvW, CURS_POINT_SELECT);
		

/*
 * change zoom button color
 */
	XtVaSetValues( w, XmNforeground, zoombc, NULL);
	XmUpdateDisplay( w );

/*
 * get two corner points
 */
	ityp = 13;
	np = 2;
	for ( ii = 0; ii < np; ii++)
		xpts[ii] = ypts[ii] = 0.0F;
	wgem_ggtpnt( sys_D, &ityp, xdev, ydev, &iret );

/*
 * check if the box is big enough
 */
	if ( fabs((double)(xdev[0] - xdev[1])) > 20. &&
		fabs((double)(ydev[0] - ydev[1])) > 20. ) {

	    wgem_gtrans(sys_D, sys_M, &np, xdev, ydev, xpts, ypts,
				&iret);

	    if ( iret == 0 ) {
		for ( ii = 0; ii < np; ii++) {
		    mapBnd.x[ii] = xpts[ii];
		    mapBnd.y[ii] = ypts[ii];
		};
	    }
	}

/*
 * Redraw the map window
 */
	fosd_plot( 1 );

/*
 * Reset the zoom button
 */
	mapw_zoomCb( w, NULL, NULL);

    }
}
