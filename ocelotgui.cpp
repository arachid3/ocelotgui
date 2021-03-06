/*
  ocelotgui -- Ocelot GUI Front End for MySQL or MariaDB

   Version: 0.4.0 Alpha
   Last modified: April 23 2015
*/

/*
  Copyright (c) 2014 by Ocelot Computer Services Inc. All rights reserved.

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301  USA
*/

/*
   The class named CodeEditor inside this program is taken and modified from
   http://qt-project.org/doc/qt-4.8/widgets-codeeditor.html
   and therefore CodeEditor's maker's copyright and BSD license provisions
   are reproduced below. These provisions apply only for the
   part of the CodeEditor class which is marked within the program.
   The program as a whole is copyrighted by Ocelot and
   licensed under GPL version 2 as stated above.
 **
 ** Copyright (C) 2014 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** $QT_BEGIN_LICENSE:BSD$
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
 **     of its contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
*/

#ifdef DEBUGGER
/*
  The routine named debug_mdbug_install inside this program is taken and modified from
  https://launchpad.net/mdbug, specifically http://bazaar.launchpad.net/~hp-mdbug-team/mdbug/trunk/view/head:/install.sql,
  and therefore MDBug's maker's copyright and GPL version 2 license provisions
  are reproduced below. These provisions apply only for the
  part of the debug_mdbug_install routine which is marked within the program.
  The program as a whole is copyrighted by Ocelot and
  licensed under GPL version 2 as stated above.

  This file is part of MDBug.

  (c) Copyright 2012 Hewlett-Packard Development Company, L.P.

  This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; version 2 of the License.

  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along with this program; if not, see
  <http://www.gnu.org/licenses>.

  Linking MDBug statically or dynamically with other modules is making a combined work based on MDBug. Thus, the terms and
  conditions of the GNU General Public License cover the whole combination.

  In addition, as a special exception, the copyright holders of MDBug give you permission to combine MDBug with free
  software programs or libraries that are released under the GNU LGPL and with code included in the standard release
  of Eclipse under the Eclipse Public License version 1.0 (or modified versions of such code, with unchanged license).
  You may copy and distribute such a system following the terms of the GNU GPL for MDBug and the licenses of the other
  code concerned, provided that you include the source code of that other code when and as the GNU GPL requires
  distribution of source code.

  Note that people who make modified versions of MDBug are not obligated to grant this special exception for their
  modified versions; it is their choice whether to do so. The GNU General Public License gives permission to release a
  modified version without this exception; this exception also makes it possible to release a modified version which
  carries forward this exception.
*/
#endif

/*
  General comments

  These are comments about the code.
  The user manual and blog comments are elsewhere; look at manual.htm
  or README.txt or ocelot.ca or ocelot.ca/blog or the repository that
  this program was downloaded from, probably github.com/ocelot-inc/ocelotgui.

  The code files are:
    ocelotgui.pro       small. used by Qt qmake
    ocelotgui.h         headers but also quite a lot of executable
    ocelotgui.cpp       executable
    install_sql.cpp     for creating debugger routines
    ocelotgui.ui        small. ui = user interface

  There are three main widgets, which generally appear top-to-bottom on
  the screen: history_edit_widget = an uncomplicated text edit which gets
  retired statements and diagnostic messages; statement_edit_widget =
  a class derived from text edit ("CodeEditor") which has code for a
  prompt on the left and for syntax highlighting; result_grid_table_widget =
  a class derived from widget ("ResultGrid") which has code for tabular
  display of a result set and manipulation thereof. The result grid is
  not based on Qt's high-level tools like QTableView, it is constructed
  from basic building blocks (scroll bars and repeatedly-occurring
  QTextEdits within QFrames). There is no use of Qt's "MySQL driver".
  The code includes a tokenizer for MySQL syntax, but not a parser.
  The coding style is generally according to MySQL coding guidelines
  http://dev.mysql.com/doc/internals/en/coding-guidelines.html
  but lines may be long, and sometimes spaces occur at ends of lines,
  and "if (x) y" may be preferred to "if (x) <newline> y".

  The code was written by Ocelot Computer Services Inc. employees, except
  for about 50 lines in the CodeEditor section (Digia copyright / BSD license),
  and except for most of the lines in install_sql.cpp (HP copyright / GPL license).
  Other contributors will be acknowledged here and/or in a "Help" display.

  The code #includes header files from MySQL/Oracle and from Qt/Digia,
  and relies on the MySQL client library and the Qt core, gui,
  and widgets libraries. Builds have been successful with several
  Linux distros and gcc 4.6. Build instructions are in the user manual or
  in a readme file

  There are many comments. Searching for the word "Todo:" in the comments
  will give some idea of what's especially weak and what's planned.

  UTF8 Conversion
  Everything which will be passed to MySQL will first be converted to UTF8.
  Todo: Ensure that sending/receiving character set is UTF8, regardless of data language.
  Todo: When receiving, only convert characters, not binary or numeric.
  Todo: Handle Qt codecs for other character sets.
  Todo: Figure out how to handle a literal preceded by an _introducer.
  A typical set of instructions will look like this and have the comment
  "See comment "UTF8 Conversion".
     int tmp_len= QStr.toUtf8().size());                  ... See how big UTF8 will be
     char *tmp= new char[tmp_len + 1];                      ... Allocate that amount
     memcpy(tmp, QStr.toUtf8().constData()), tmp_len + 1);  ... Copy including trail '\0' byte
     ... And later, say "delete []tmp" or hope for garbage collection

  Todo: Allow right-click for changing individual widgets.

  Todo: Some more checks for garbage collection, then some equivalent to valgrind.

  The usual ways to build are described in README.txt.
  An unusual way would be with Qt 4.8 source-code libraries supplied by Digia:
      Download 4.8 libraries via http://qt-project.org/downloads
         This is a source download; you'll need to do ./configure and make.
         Follow instructions in http://qt-project.org/doc/qt-4.8/install-x11.html
         After make and make install you'll have /usr/local/Trolltech/Qt-4.8.6
         At this point I got totally stuck with Qt Creator -- it refused to try to build with Qt 4.8.
         And yes, I changed the kit, I changed the paths, but it was all a waste of a lot of time.
         Well, if anyone else succeeds, let me know.
         But I "succeeded" by exiting Qt Creator, creating a new make file, and building from the shell.
         (I copy and then remove ui_ocelotgui.h, which may or may not have some importance that I missed.)
         cd ~/ocelotgui
         mv ui_ocelotgui.h ui_ocelotgui.h.bak
         /usr/local/Trolltech/Qt-4.8.6/bin/qmake ocelotgui.pro
         make
        ./ocelotgui
        That ruins what Qt Creator expects when I get back to Qt Cretaor, so I get back to 5.1 with:
        cd ~/ocelotgui
        /home/pgulutzan/Qt/5.1.1/gcc_64/bin/qmake ./ocelotgui.pro
        mv ui_ocelotgui.h ui_ocelotgui.h.bak
        rm ui_ocelotgui.h
        make (ignore failure)
        re-enter Qt Creator. Build | Clean all. Build all. Run.
*/




#include "ocelotgui.h"
#include "ui_ocelotgui.h"


/* Whenever you see STRING_LENGTH_512, think: here's a fixed arbitrary allocation, which should be be fixed up. */
#define STRING_LENGTH_512 512

/* Connect arguments and options */
  /* todo: I doubt we're handling the "loose" or "enabled" possibilities */
  static char* ocelot_host_as_utf8= 0;                  /* --host=s */
  static char* ocelot_database_as_utf8= 0;              /* --database=s */
  static char* ocelot_user_as_utf8= 0;                  /* --user=s */
  static char* ocelot_password_as_utf8= 0;              /* --password[=s] */
  static unsigned short ocelot_port= MYSQL_PORT;        /* --port=n */
  static char* ocelot_unix_socket_as_utf8= 0;           /* --socket=s */
  static unsigned int ocelot_protocol_as_int= 0;        /* --protocol=s for MYSQL_OPT_PROTOCOL */
  static char* ocelot_init_command_as_utf8= 0;          /* --init_command=s for MYSQL_INIT_COMMAND */
  /* Connect arguments below this point are minor and many are unsupported. */
  static unsigned short ocelot_auto_rehash= 1;             /* --auto_rehash */
  static unsigned short ocelot_auto_vertical_output= 0;    /* --auto_vertical_output */
  static unsigned short ocelot_batch= 0;                   /* --batch */
  static unsigned short ocelot_binary_mode= 0;             /* --binary_mode */
  /* QString ocelot_bind_address */                        /* --bind_address=s */
  static char* ocelot_set_charset_dir_as_utf8= 0;         /* --character_sets_dir=s for MYSQL_SET_CHARSET_DIR */
  static unsigned short ocelot_result_grid_column_names= 1;/* --column_names */
  static unsigned short ocelot_column_type_info= 0;        /* --column_type_info */
  static unsigned short ocelot_comments= 0;               /* --comments */
  static unsigned short ocelot_opt_compress= 0;           /* --compress for MYSQL_OPT_COMPRESS */
  static unsigned short ocelot_connect_expired_password= 0;/* --connect_expired_password */
  static unsigned long int ocelot_opt_connect_timeout= 0;  /* --connect_timeout = n for MYSQL_OPT_CONNECT_TIMEOUT */
  /* QString ocelot_debug */                               /* --debug[=s] */
  static unsigned short ocelot_debug_check= 0;             /* --debug_check */
  static unsigned short ocelot_debug_info= 0;              /* --debug_info */
  static char* ocelot_default_auth_as_utf8= 0;      /* --default_auth=s for MYSQL_DEFAULT_AUTH */
  static char* ocelot_set_charset_name_as_utf8= 0;  /* --default_character_set=s for MYSQL_SET_CHARSET_NAME */
  /* exists as QString */                    /* --defaults_extra_file=s */
  /* exists as QString */                    /* --defaults_file=s */
  /* QString ocelot_defaults_group_suffix */ /* --defaults_group_suffix=s */
  /* exists as QString */                    /* --delimiter=s */
  static unsigned short ocelot_enable_cleartext_plugin= 0;  /* --enable_cleartext_plugin for MYSQL_ENABLE_CLEARTEXT_PLUGIN */
  /* QString ocelot_execute */               /* --execute=s */
  static unsigned short ocelot_force= 0;     /* --force */
  static unsigned short ocelot_help= 0;      /* --help */
  static unsigned short ocelot_histignore= 0;/* --histignore=s */
  static unsigned short ocelot_html= 0;      /* --html */
  static unsigned short ocelot_ignore_spaces= 0;  /* --ignore_spaces */
  static unsigned short ocelot_line_numbers= 0;   /* --line_numbers */
  static unsigned short ocelot_opt_local_infile= 0;    /* --local_infile[=n]  for MYSQL_OPT_LOCAL_INFILE */
  /* QString ocelot_login_path */            /* --login_path=s */
  static unsigned long int ocelot_max_allowed_packet= 16777216; /* --max_allowed_packet=n */
  static unsigned long int ocelot_max_join_size= 1000000; /* --max_join_size = n */
  static unsigned short ocelot_named_commands= 0;         /* --named_commands */
  static unsigned long int ocelot_net_buffer_length= 16384; /* --net_buffer_length=n */
  static unsigned short ocelot_no_beep= 0;                /* --no_beep */
  static unsigned short ocelot_no_defaults= 0;            /* --no_defaults */
  static unsigned short ocelot_one_database= 0;           /* --one-database */
  /* QString ocelot_pager */                              /* --pager[=s] */
  static unsigned short ocelot_pipe= 0;                   /* --pipe */
  static char* ocelot_plugin_dir_as_utf8= 0;        /* --plugin_dir=s for MYSQL_PLUGIN_DIR */
  static unsigned short ocelot_print_defaults= 0;   /* --print_defaults */
  /* QString ocelot_prompt */                       /* --prompt=s */
  static unsigned short ocelot_quick= 0;            /* --quick */
  static unsigned short ocelot_raw= 0;              /* --raw */
  static unsigned int ocelot_opt_reconnect= 0;      /* --reconnect for MYSQL_OPT_RECONNECT */                                           /* --reconnect */
  static unsigned short ocelot_safe_updates= 0;           /* --safe-updates or --i-am-a-dummy */
  static unsigned short ocelot_secure_auth= 1;             /* --secure_auth for MYSQL_SECURE_AUTH (default=true if version >= 5.6.5) */
  static unsigned long int ocelot_select_limit= 0;  /* --select_limit = n */
  static char* ocelot_server_public_key_as_utf8= 0; /* --server_public_key=s for MYSQL_SERVER_PUBLIC_KEY */
  static char* ocelot_shared_memory_base_name_as_utf8= 0; /* --shared_memory_base_name=s for MYSQL_SHARED_MEMORY_BASE_NAME */
  static unsigned short ocelot_history_includes_warnings= 0; /* --show_warnings include warning(s) returned from statement? default = no. */
  static unsigned short ocelot_sigint_ignore= 0;          /* --sigint_ignore */
  static unsigned short ocelot_silent= 0;                 /* --silent */
  static char* ocelot_opt_ssl_as_utf8= 0;           /* --ssl for CONNECT */
  static char* ocelot_opt_ssl_ca_as_utf8= 0;        /* --ssl-ca for MYSQL_OPT_SSL_CA */
  static char* ocelot_opt_ssl_capath_as_utf8= 0;    /* --ssl-capath for MYSQL_OPT_SSL_CAPATH. */
  static char* ocelot_opt_ssl_cert_as_utf8= 0;      /* --ssl-cert for MYSQL_OPT_SSL_CERT */
  static char* ocelot_opt_ssl_cipher_as_utf8= 0;    /* --ssl-cipher for MYSQL_OPT_SSL_CIPHER */
  static char* ocelot_opt_ssl_crl_as_utf8= 0;       /*  --ssl-crl for MYSQL_OPT_SSL_CRL */
  static char* ocelot_opt_ssl_crlpath_as_utf8= 0;   /* --ssl-crlpath for MYSQL_OPT_SSL_CRLPATH */
  static char* ocelot_opt_ssl_key_as_utf8= 0;       /* --ssl-key for MYSQL_OPT_SSL_KEY */
  static unsigned short int ocelot_opt_ssl_verify_server_cert= 0;  /* --ssl-verify-server-cert for MYSQL_OPT_SSL_VERIFY_SERVER_CERT. --ssl-verify-server-cert (5.7) */
  static unsigned short ocelot_syslog= 0;           /* --syslog (5.7) */
  static unsigned short ocelot_table= 0;            /* --table */
  /* todo: ocelot_history_tee_file_name exists as QString. Fix that. */
  static bool ocelot_history_tee= false;            /* --tee for tee */
  static unsigned short ocelot_unbuffered= 0;       /* --unbuffered */
  static unsigned short ocelot_verbose= 0;          /* --verbose */
  static unsigned short ocelot_version= 0;          /* --version */
  static unsigned short ocelot_result_grid_vertical= 0;   /* --vertical for vertical */
  static unsigned short ocelot_wait= 0;                   /* --wait ... actually this does nothing */
  static unsigned short ocelot_xml= 0;                    /* --xml */

  /* Some items we allow, which are not available in mysql client */
  static char* ocelot_opt_bind_as_utf8= 0;              /* for MYSQL_OPT_BIND */
  static unsigned short ocelot_opt_can_handle_expired_passwords= 0;/* for MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS */
  static char* ocelot_opt_connect_attr_delete_as_utf8= 0;  /* for MYSQL_OPT_CONNECT_ATTR_DELETE */
  static unsigned short int ocelot_opt_connect_attr_reset= 0; /* for MYSQL_OPT_CONNECT_ATTR_RESET */
  static char* ocelot_read_default_file_as_utf8= 0; /* for MYSQL_READ_DEFAULT_FILE */
  static char* ocelot_read_default_group_as_utf8= 0;/* for MYSQL_READ_DEFAULT_GROUP */
  static unsigned int ocelot_opt_read_timeout= 0;          /* for MYSQL_OPT_READ_TIMEOUT */
  static unsigned short int ocelot_report_data_truncation= 0; /* for MYSQL_REPORT_DATA_TRUNCATION */
  static unsigned short int ocelot_opt_use_result= 0; /* for MYSQL_OPT_USE_RESULT */

  /* Some items we allow, but the reasons we allow them are lost in the mists of time */
  /* I gather that one is supposed to read the charset file. I don't think we do. */
  static unsigned short ocelot_opt_named_pipe;          /* for MYSQL_OPT_NAMED_PIPE */
  static unsigned int ocelot_opt_write_timeout= 0; /* for MYSQL_OPT_WRITE_TIMEOUT */



/* copy of an information_schema.columns select, used for rehash */
  static  unsigned int rehash_result_column_count= 0;
  static unsigned int rehash_result_row_count= 0;
  static char *rehash_mysql_res_copy= 0;       /* gets a copy of mysql_res contents, if necessary */
  static char **rehash_mysql_res_copy_rows= 0; /* dynamic-sized list of mysql_res_copy row offsets, if necessary */

  int options_and_connect(unsigned int connection_number);

/* Global mysql definitions */
#define MYSQL_MAIN_CONNECTION 0
#define MYSQL_DEBUGGER_CONNECTION 1
#define MYSQL_KILL_CONNECTION 2
#ifdef DEBUGGER
  MYSQL mysql[3];
  static int connected[3]= {0, 0, 0};
#else
  MYSQL mysql[1];
  static int connected[1]= {0};
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w (argc, argv);
    w.showMaximized();
    return a.exec();
}


MainWindow::MainWindow(int argc, char *argv[], QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::MainWindow)
{
  /* Initialization */

  ui->setupUi(this);              /* needed so that the menu will show up */

  setWindowTitle("ocelotgui");

  is_mysql_connected= 0;
  mysql_res= 0;

  /* client variable defaults */
  /* todo: these should be set up earlier in connect_mysql_options_2 because users should be able to pick them as options */
  ocelot_dbms= "mysql";
  ocelot_grid_detached= 0;
  ocelot_grid_max_row_lines= 5; /* obsolete? */               /* maximum number of lines in 1 row. warn if this is exceeded? */
  connect_mysql_options_2(argc, argv);               /* pick up my.cnf and command-line MySQL-related options, if any */

  ocelot_statement_prompt_background_color= "lightGray"; /* set early because create_widget_statement() depends on this */


  ocelot_grid_border_color= "black";
  ocelot_grid_header_background_color= "lightGray";
  ocelot_grid_cell_right_drag_line_color= "lightBlue";
  ocelot_grid_cell_border_color= "black";
  ocelot_grid_cell_border_size= "1";
  ocelot_grid_cell_right_drag_line_size= "5";
  /* Probably result_grid_table_widget_saved_font only matters if the connection dialog box has to go up. */
  QFont tmp_font;
//  QFont *saved_font;
  tmp_font= this->font();
//  saved_font=&tmp_font;
//  result_grid_table_widget= new ResultGrid(0, saved_font, this);

  /* Todo: I used to say "(this)" i.e. have mainwindow parent. Maybe setParent later? */
  result_grid_tab_widget= new QTabWidget48();
  result_grid_tab_widget->hide();
  for (int i_r= 0; i_r < RESULT_GRID_TAB_WIDGET_MAX; ++i_r)
  {
    result_grid_table_widget[i_r]= new ResultGrid(this);
    result_grid_tab_widget->addTab(result_grid_table_widget[i_r], QString::number(i_r + 1));
    result_grid_table_widget[i_r]->hide(); /* Maybe this isn't necessary */
  }
  for (int i_r= 0; i_r < RESULT_GRID_TAB_WIDGET_MAX; ++i_r)
  {
    result_grid_table_widget[i_r]->installEventFilter(this); /* must catch fontChange, show, etc. */
    result_grid_table_widget[i_r]->grid_vertical_scroll_bar->installEventFilter(this);
  }
  main_layout= new QVBoxLayout();               /* todo: do I need to say "this"? */

  create_widget_history();
  create_widget_statement();
#ifdef DEBUGGER
  create_widget_debug();
#endif

  main_window= new QWidget();

  /*
    Defaults for items that can be changed with Settings menu item.
    Todo: These should come from current values, after processing of any Qt options on the command line.
          We already get "current values" somewhere.
  */
  set_current_colors_and_font(); /* set ocelot_statement_color, ocelot_grid_color, etc. */
  ocelot_statement_border_color= "black";

  ocelot_statement_highlight_literal_color= "green";
  ocelot_statement_highlight_identifier_color= "blue";
  ocelot_statement_highlight_comment_color= "red";
  ocelot_statement_highlight_operator_color= "lightGreen";
  ocelot_statement_highlight_reserved_color= "magenta";

  ocelot_history_border_color= "black";
  ocelot_main_border_color= "black";

  make_style_strings();
  main_layout->addWidget(history_edit_widget);
  main_layout->addWidget(result_grid_tab_widget);
  main_layout->addWidget(statement_edit_widget);
#ifdef DEBUGGER
  main_layout->addWidget(debug_top_widget);
#endif

  main_window->setLayout(main_layout);
  setCentralWidget(main_window);

  create_menu();    /* Do this at a late stage because widgets must exist before we call connect() */

  create_the_manual_widget(); /* Make the text for Help | The Manual */

  /*
    If the command-line option was -p but not a password, then password input is necessary
    so put up the connection dialog box. Otherwise try immediately to connect.
    Todo: better messages so the user gets the hint: connection is necessary / password is necessary.
  */
  if (ocelot_password_was_specified == 1)
  {
    statement_edit_widget->insertPlainText("CONNECT");
    action_execute();
  }
  else
  {
    action_connect();
  }
  statement_edit_widget->setFocus(); /* Show user we're ready to accept a statement in the statement edit widget */
}


MainWindow::~MainWindow()
{
  delete ui;
}


/*
  Create statement_edit_widget.
  All user SQL input goes into statement_edit_widget.
  This will be a CodeEditor, which is a class derived from QTextEdit, with a prompt on the left.
  Todo: Check: after font change or main window resize, are line numbers and text in sync?
*/
void MainWindow::create_widget_statement()
{
  statement_edit_widget= new CodeEditor();
  statement_edit_widget->setLineWrapMode(QPlainTextEdit::NoWrap);
  /* statement_edit_widget->setAcceptRichText(false); */ /* Todo: test whether this works */
  connect(statement_edit_widget->document(), SIGNAL(contentsChanged()), this, SLOT(action_statement_edit_widget_text_changed()));
  statement_edit_widget_text_changed_flag= 0;

  /*
    Defaults.
    The prompt can be changed with a "prompt" statement or by my.cnf contents.
    The widget-left i.e. prompt bgcolor can be changed with a menu item.
  */
  statement_edit_widget->statement_edit_widget_left_bgcolor= QColor(ocelot_statement_prompt_background_color);
  statement_edit_widget->statement_edit_widget_left_treatment1_textcolor= QColor(ocelot_statement_color);

  statement_edit_widget->prompt_default= ocelot_prompt;     /* Todo: change to "\N [\d]>"? */
  statement_edit_widget->prompt_as_input_by_user= statement_edit_widget->prompt_default;

  statement_edit_widget->statement_count= 0;
  statement_edit_widget->dbms_version= (QString)"";
  statement_edit_widget->dbms_database= (QString)"";
  statement_edit_widget->dbms_port= (QString)"";
  statement_edit_widget->dbms_host= (QString)"";
  statement_edit_widget->dbms_current_user= (QString)"";
  statement_edit_widget->dbms_current_user_without_host= (QString)"";
  statement_edit_widget->delimiter= ocelot_delimiter_str;
  statement_edit_widget->result= (QString)"";

  /*
    Problem: the above statements don't cause recalculation of width of prompt on the left.
    (Original calculation happens when widget is created.)
    I can force recalculation by doing an "emit" here.
    But I'm not sure this is the right way to do it.
  */
  emit statement_edit_widget->update_prompt_width(0);

  statement_edit_widget->installEventFilter(this);
}


/*
  The event filter, for detecting:
    Has the user pressed on the vertical scroll bar of result_grid_widget?
    Has the user pressed Enter or return on statement_edit_widget?
      If the user presses Enter or Return on statement_edit_widget,
      and the last non-comment token is the delimiter (normally ";" unless DELIMITER happened),
      and the cursor is at the end i.e. after the delimiter,
      that should cause execution.
      Otherwise return false ...
      I've seen examples where, instead of "return false;", the
      instruction is "return QMainWindow::eventFilter(obj, event);".
      I think the idea there is to go direct to the main processor
      for text editing, bypassing other event filters.
  Todo: Consider: Perhaps this should not be in Main>Window:: but in CodeEditor::.
  Todo: Consider: Perhaps this should be a menu item, not a filter event.
                  (There's already a menu item, but it's not for Enter|Return.)
  There are a few "Ocelot keyword" items that do not require ";" or delimiter
  provided they're the first word, for example "QUIT".
*/
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
  QString text;

//  if (obj == result_grid_table_widget[0]->grid_vertical_scroll_bar)
//  {
//    /* BING */
//    /*
//      Probably some of these events don't cause scroll bar value to change,
//      I've only seen that happen for MouseMove and MouseButtonRelease.
//      But that's harmless, it only means we call scroll_event for nothing.
//    */
//    if ((event->type() == QEvent::KeyPress)
//    ||  (event->type() == QEvent::KeyRelease)
//    ||  (event->type() == QEvent::MouseButtonDblClick)
//    ||  (event->type() == QEvent::MouseButtonPress)
//    ||  (event->type() == QEvent::MouseButtonRelease)
//    ||  (event->type() == QEvent::MouseMove)
//    ||  (event->type() == QEvent::MouseTrackingChange)) return (result_grid_table_widget->scroll_event());
//  }


  for (int i_r= 0; i_r < RESULT_GRID_TAB_WIDGET_MAX; ++i_r)
  {
    if (obj == result_grid_table_widget[i_r])
    {
      if (event->type() == QEvent::FontChange) return (result_grid_table_widget[i_r]->fontchange_event());
      if (event->type() == QEvent::Show) return (result_grid_table_widget[i_r]->show_event());
    }
    if (obj == result_grid_table_widget[i_r]->grid_vertical_scroll_bar)
    {
      return (result_grid_table_widget[i_r]->vertical_scroll_bar_event());
    }
  }


#ifdef DEBUGGER
  for (int debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    if (obj == debug_widget[debug_widget_index])
    {
      if (event->type() == QEvent::MouseButtonPress)
      {
        action_debug_mousebuttonpress(event, debug_widget_index);
        return false;
      }
    }
  }
#endif

  if (obj != statement_edit_widget) return false;
  if (event->type() != QEvent::KeyPress) return false;
  QKeyEvent *key= static_cast<QKeyEvent *>(event);

  if ((key->key() == Qt::Key_Tab) && (ocelot_auto_rehash > 0))
  {
    int i= 0;
    QString s;
    char search_string[512];
    int search_string_len;
    while (main_token_lengths[i] != 0) ++i;
    if (i > 1)
    {
      i-= 1;
      if ((main_token_lengths[i] < 64) && (main_token_types[i] >= TOKEN_TYPE_OTHER))
      {
        text= statement_edit_widget->toPlainText();
        s= text.mid(main_token_offsets[i], main_token_lengths[i]);
        search_string_len= s.toUtf8().size();
        memcpy(search_string, s.toUtf8().constData(), search_string_len);
        search_string[search_string_len]= '\0';
        rehash_search(search_string);
        if (strcmp(search_string, "") > 0)
        {
          strcpy(search_string, search_string + search_string_len);
          statement_edit_widget->insertPlainText(search_string);
        }
      }
    }
  }

  if ((key->key() != Qt::Key_Enter) && (key->key() != Qt::Key_Return)) return false;
  /* No delimiter needed if first word in first statement of the input is an Ocelot keyword e.g. QUIT */
  /* No delimiter needed if Ctrl+Enter, which we'll regard as a synonym for Ctrl+E */
  if ((main_statement_type >= TOKEN_KEYWORD_QUESTIONMARK) || (key->modifiers() & Qt::ControlModifier))
  {
    emit action_execute();
    return true;
  }
  text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */
  int i= 0;
  while (main_token_lengths[i] != 0) ++i;
  QString s= "";
  /*
    Todo: I think the following was written before there was detection of reserved words + Ocelot keywords.
    If so, token_type() doesn't need to be invoked, because main_token_types[] has the result already.
  */
  while (i > 0)
  {
    --i;
    s= text.mid(main_token_offsets[i], main_token_lengths[i]);
    int t= token_type(s.data(), main_token_lengths[i]);
    if (t == TOKEN_TYPE_COMMENT_WITH_SLASH) continue;
    if (t == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE) continue;
    if (t == TOKEN_TYPE_COMMENT_WITH_MINUS) continue;
    break;
  }

  if (statement_edit_widget->textCursor().position() <= main_token_offsets[i]) return false;

  /* "go" or "ego", alone on the line, if --named-commands, is statement end */
  if ((ocelot_named_commands > 0)
  && ((main_token_types[i] == TOKEN_KEYWORD_GO) || (main_token_types[i] == TOKEN_KEYWORD_EGO)))
  {
    QString q;
    for (int i_off= main_token_offsets[i] - 1;; --i_off)
    {
      if (i_off < 0) q= "\n";
      else q= text.mid(i_off, 1);
      if ((q == "\n") || (q == "\r"))
      {
        emit action_execute();
        return true;
      }
      if (q != " ") break;
    }
  }

  /* if create-routine && count-of-ENDs == count-of-BEGINS then END is the end else ; is not the end */
  int returned_begin_count;
  if (ocelot_delimiter_str != ";") returned_begin_count= 0;
  else
  {
    get_next_statement_in_string(0, &returned_begin_count);
    if ((returned_begin_count == 0) && (QString::compare(s, "end", Qt::CaseInsensitive) == 0))
    {
      emit action_execute();
      return true;
    }
  }


  if (s != ocelot_delimiter_str) return false;

  if (returned_begin_count > 0) return false;

  /* All conditions have been met. Execute, and eat the return key. */
  emit action_execute();
  return true;
}


/*
  History
  =======

  The history widget history_edit_widget is an editable QTextEdit
  which contains retired statements + errors/warnings, scrolling
  so it looks not much different from the mysql retired-statement
  scrolling. However, our history does not include result sets.
  These user-settable variables affect history_edit_widget:
  ocelot_history_includes_prompt                   default yes
  ocelot_history_text_select_includes_prompt       default yes
  ocelot_history_max_lines                         default 10000. 0 means it's suppressed.
  ocelot_history_text_font                         default unknown
  ocelot_history_error_font                        default unknown
  ocelot_history_prompt_font                       default unknown
  ocelot_history_detached                          default no
  ocelot_history_includes_warnings                 default no

  The statement is always followed by an error message,
  but @ocelot_history_includes_warnings is affected by ...
  warnings  (\W) Show warnings after every statement.
  nowarning (\w) Don't show warnings after every statement.
  If the prompt is included, then we should be saving time-of-day
  for the first statement-line prompt and doing prompt_translate()
  for each line of the statement when we copy it out.
  In mysql client it's possible to say up-arrow and get a prior statement,
  but we have up-arrow for different purposes, probably PgUp|PgDown is better,
  or alt+up-arrow|alt+down-arrow.
  I didn't see a need to make the widget with a subclass of QTextEdit.
  The history_edit_widget is QTextEdit, differing from statement_edit_widget
  which is CodeEditor which is derived from QPlainTextEdit.

  History menu items / commands:
  * The usual edit menu items = cut, copy, paste, etc.
    Therefore we don't really need to limit history size, users can clear.
  * PgUp + PgDn
    * are menu items for "previous statement" / "next statement"
    * disable if there's no previous / next
    * if done on statement widget, brings in from history -- dunno if history should scroll when that happens
    ? when bringing in from history to statement, don't re-execute
    * if user executes a restated statement, there's a behaviour choice: now the restated statement is last statement, or now the last-picked statement is still last statement
  * Open | Save | Close | Delete
    * file items, with the intent of keeping history
    * you can go back in history by going back in a file
    * format should be the same as MySQL log history
      ? but in that case, should display look like MySQL log too?
    * Save means "save what's being shown, so it's affected by Hide
  * Find | Goto
    * you need these menu items for history soon, so might as well have them for statements too
  * Hide prompt + Hide result + Hide result set
  + Settings: History Widget: Colors and Fonts has different colors for prompt/statement/result/result set

  Comments unrelated to HTML work:
  Possible difficulty: our history is statement-at-a-time, not screen-at-a-time", if there are multi statements
  Certain statements might be suppressed
    * should Quit and Source go in history?
    * should only data-change statements go in history?
    * should client-statements be suppressed?
    * this is a "Filter" matter
    ? to what extent is this useful for audit?
    Wherever there's a menu item, there should also be a client-statement
    * history must include comments even if they're not sent to the server (there's some mysql-option for that)
    * Delimiter might change, so when you copy from history to statement, what should you do?
    * When the debugger comes in, statements done by the debugger are a special history category

  HTML in the history widget
  Unlike statement_edit_widget, history_edit_widget is QTextEdit and allows rich text.
  History is an editable QTextEdit which contains HTML.
  Each history entry has: prompt, statement, result, and possibly result set.
  (Result set is unimplemented but will be mentioned in these comments;
  eventually it will be something that appears like mysql client result set.)
  There are two kinds of markup:
  (1) <span style="...">...</span> for changes to color + font.
      Currently this is only used for history prompt bgcolor = statement prompt bgcolor.
  (2) <a name='[mark]'> for showing what kind of text follows.
      The markups are <a name='STATEMENT START'> ... <a name='STATEMENT END'>
                <a name='PROMPT START'> ... <a name='PROMPT END'>
                <a name='RESULT'> (ends when statement ends)
                <a name='ENTITY'> (always an entity for a single & character)
      Digia does not document or guarantee what it will do with <a>
      and we're not using it for its intended function as anchor.
      Example: "SELECT * FROM t <br> WHERE x < '&';" becomes
      <a name='STATEMENT START'>
      <span ...><a name='PROMPT'>mysql&gt;<a name='PROMPT END'></span>
      SELECT * FROM t <br>
      <a name='PROMPT'>&gt;<a name='PROMPT END'>
      WHERE a &lt; '<a name='ENTITY'>&amp;';
      <span ...><a name='RESULT'>OK</span>
      <a name='STATEMENT END'>
  Handle PgUp | PgDn by picking up next | previous STATEMENT START ... END.
  When copying to history, change < and > and & to entities but mark if '&'.
    Also: change ' and " to entities.
  When copying from history, change entities to < and > and & unless marked.
  Todo: hide prompt + result by putting whole thing within <a name='...'>.
  In order to see the HTML, say:
    QMessageBox msgBox;
    msgBox.setTextFormat(Qt::PlainText);
    msgBox.setText(history_edit_widget->toHtml());
    msgBox.exec();
  History is editable.
    If the user inputs "<" when editing the history,
    change that to an entity immediately -- but maybe Qt will do that.
  Idea: use QTextCursor to change cursor in history if PgUp|PgDn in statement.
  Idea: multiple types of result: warning, error, result set
*/

void MainWindow::create_widget_history()
{
  history_edit_widget= new QTextEdit();
  history_edit_widget->setReadOnly(false);       /* if history shouldn't be editable, set to "true" here */
  history_edit_widget->hide();                   /* hidden until a statement is executed */
  history_markup_make_strings();
  return;
}


/*
  Set the strings that will be used for markup comments in history widget.
  Call history_markup_make_strings() at program start.
  Todo: At the moment these could be constants but I don't think they always will be.
  Note: <br> in history_markup_statement_start is necessary; else prompt_start is ignored.
*/
void MainWindow::history_markup_make_strings()
{
  history_markup_statement_start= "<a name=\"STATEMENT START\"></a><br>";
  history_markup_statement_end=   "<a name=\"STATEMENT END\"></a>";
  history_markup_prompt_start=    "<a name=\"PROMPT START\"></a>";
  history_markup_prompt_end=      "<a name=\"PROMPT END\"></a>";
  history_markup_result=          "<a name=\"RESULT\"></a>";
  history_markup_entity=          "<a name=\"ENTITY\"></a>";
}


/* The following will append the statement to history, line-at-a-time with prompts. */
/* It seems to work except that the prompt is not right-justified. */
/* Todo: right justify. Make it optional to show the prompt, unless prompt can be hidden. */
void MainWindow::history_markup_append()
{
  QString plainTextEditContents;
  QStringList statement_lines;
  int statement_line_index;
  QString history_statement;

  plainTextEditContents= query_utf16; /* Todo: consider: why bother copying rather than using query_uitf16? */
  statement_lines= plainTextEditContents.split("\n");
  //statement_line_index= 0;                                                          /* Todo throw this useless line away */
  /* Todo: There should be a better way to ensure that Qt realizes the whole widget is rich text. */
  /* Todo: Some of this could be at start of history_edit_widget but what would happen if I cleared the whole area? */
  /* Todo: background-color of prompt could be settable for history widget, rather than = statement background color. */
  /* Todo: there's a weird bug that splits prompt into 'm' and 'ysql>', but the output still looks okay. */
  history_statement= "<i></i>";                                           /* hint that what's coming is HTML */
  history_statement.append(history_markup_statement_start);

  for (statement_line_index= 0; statement_line_index < statement_lines.count(); ++statement_line_index)
  {
    history_statement.append("<span style=\" background-color:");
    history_statement.append(ocelot_statement_prompt_background_color);
    history_statement.append(";\">");
    history_statement.append(history_markup_prompt_start);
    history_statement.append(history_markup_copy_for_history(statement_edit_widget->prompt_translate(statement_line_index + 1)));
    history_statement.append(history_markup_prompt_end);
    history_statement.append("</span>");
    history_statement.append(history_markup_copy_for_history(statement_lines[statement_line_index]));
    history_statement.append("<br>");
  }

  history_statement.append(history_markup_result);
  history_statement.append(history_markup_copy_for_history(statement_edit_widget->result)); /* the main "OK" or error message */
  history_statement.append(history_markup_statement_end);

  history_edit_widget->append(history_statement);

  history_markup_counter= 0;

  history_tee_write(query_utf16); /* not related to markup, just a convenient place to call */
  history_tee_write(statement_edit_widget->result);
}


/* When copying to history, change < and > and & and " to entities. */
/* Change on 2015-03-16: change newline to <br>. */
QString MainWindow::history_markup_copy_for_history(QString inputs)
{
  QString outputs;
  QString c;
  int i;

  outputs= "";
  for (i= 0; i < inputs.length(); ++i)
  {
    c= inputs.mid(i, 1);
    if (c == "<") c= "&lt;";
    if (c == ">") c= "&gt;";
    if (c == "&") c= "&amp;";
    if (c == "\"") c= "&quot;";
    if (c == "\n") c= "<br>";
    outputs.append(c);
  }
  return outputs;
}


/* In response to edit menu "Previous statement" or "Next statement",
   make statement widget contents = a previous statement from history,
   without executing.
   When copying from history, change entities to < and > and & and "
   But the job is much bigger than that.
   Find "STATEMENT START".
   Start copying.
   Skip everything between "PROMPT START" and "PROMPT END".
   Stop copying when you see "RESULT".
*/
void MainWindow::history_markup_previous()
{
  ++history_markup_counter;
  if (history_markup_previous_or_next() == -1) --history_markup_counter;
}


void MainWindow::history_markup_next()
{
  --history_markup_counter;
  if (history_markup_previous_or_next() == -1) ++history_markup_counter;
}


int MainWindow::history_markup_previous_or_next()
{
  QString outputs;
  QString final_outputs;
  int index_of_prompt_end;
  int index_of_a_gt, index_of_lt_a;
  int i;
  QString c;
  int search_start_point;

  QString s;
  int index_of_statement_start;
  int index_of_result;

  s= history_edit_widget->toHtml();
  search_start_point= -1;                                                    /* search starting from end of string */
  for (i= 0;;)
  {
    index_of_statement_start= s.lastIndexOf("\"STATEMENT START\"", search_start_point);
    if (index_of_statement_start == -1) return -1;                           /* (statement start not found) */
    index_of_result= s.indexOf("\"RESULT\"", index_of_statement_start);
    if (index_of_result == -1) return -1;                                    /* (result not found) */
    ++i;
    if (i >= history_markup_counter) break;                                  /* (we're at the right SELECT) */
    search_start_point= index_of_statement_start - 1;                        /* keep going back */
  }
  outputs= "";

  s= s.mid(index_of_statement_start, index_of_result - index_of_statement_start);

  /* At this point s = whatever's between statement start and result */
  /* But there might be a series of repeat(prompt-start prompt-end real-statement) till result */
  for (;;)
  {
    index_of_prompt_end= s.indexOf("\"PROMPT END\"", 0);
    if (index_of_prompt_end == -1) break;
    index_of_a_gt= s.indexOf("a>", index_of_prompt_end) + 2;
    if (index_of_a_gt == -1) break;
    index_of_lt_a= s.indexOf("<a", index_of_a_gt);
    if (index_of_lt_a == -1) break;
    outputs.append(s.mid(index_of_a_gt, index_of_lt_a - index_of_a_gt));
    s= s.mid(index_of_a_gt, -1);
  }

  /* At this point outputs = the statement but it might contain entities
     so reverse whatever happened in history_markup_copy_for_history(),
     and change <br> back to \r.
     Todo: check what happens if original statement contains an entity.
  */
  final_outputs= "";
  for (i= 0; i < outputs.length(); ++i)
  {
    if (outputs.mid(i, 4) == "<br>")
    {
      final_outputs.append("\r");
      i+= 3;
    }
    else if (outputs.mid(i, 6) == "<br />")
    {
      final_outputs.append("\r");
      i+= 5;
    }
    else if (outputs.mid(i, 4) == "&lt;")
    {
      final_outputs.append("<");
      i+= 3;
    }
    else if (outputs.mid(i, 4) == "&gt;")
    {
      final_outputs.append(">");
      i+= 3;
    }
    else if (outputs.mid(i, 5) == "&amp;")
    {
      final_outputs.append("&");
      i+= 4;
    }
    else if (outputs.mid(i, 6) == "&quot;")
    {
      final_outputs.append("\"");
      i+= 5;
    }
    else
    {
      c= outputs.mid(i, 1);
      final_outputs.append(c);
    }
  }

  statement_edit_widget->setPlainText(final_outputs);
  return 0;
}


/*
  for tee
  -------
  * Code related to tee should have the comment somewhere = "for tee"
  * bool ocelot_history_tee initially is false
  * the options --tee=filename and --no-tee exist and are checked (I think)
  * the client statements tee filename and notee will be seen
  * there are no menu options (todo: decide whether this is a flaw)
  * apparently the mysql client would flush, but we don't (todo: maybe change this)
  * the mysql client would include results from queries, but we don't (todo: this is a flaw)
  * there might be html in the output (todo: decide whether this is a flaw)
*/
void MainWindow::history_tee_write(QString text_line)  /* for tee */
{
  if (ocelot_history_tee)                              /* for tee */
  {
    QString s= text_line;
    int query_len= s.toUtf8().size();                  /* See comment "UTF8 Conversion" */
    char *query= new char[query_len + 1];
    memcpy(query, s.toUtf8().constData(), query_len + 1);
    query[query_len]= '\0';                            /* todo: think: is this necessary? */
    fputs(query, ocelot_history_tee_file);
    fputs("\n", ocelot_history_tee_file);
    fflush(ocelot_history_tee_file);
    delete []query;
  }
}


int MainWindow::history_tee_start(QString file_name) /* for tee */
{
  int query_len= file_name.toUtf8().size();                  /* See comment "UTF8 Conversion" */
  char *query= new char[query_len + 1];
  memcpy(query, file_name.toUtf8().constData(), query_len + 1);
  query[query_len]= '\0';                            /* todo: think: is this necessary? */
  ocelot_history_tee_file= fopen(query, "a");        /* Open specified file, read only */
  delete []query;
  if (ocelot_history_tee_file != NULL)
  {
    ocelot_history_tee= true;
    ocelot_history_tee_file_name= file_name;
    return 1;
  }
  return 0;
}


void MainWindow::history_tee_stop()                 /* for tee */
{
  fclose(ocelot_history_tee_file);
  ocelot_history_tee= false;
}


/*
  Create the menu.
  Apparently the menubar already exists because of the call to UiSetup() earlier. Incomprehensible.
  Todo: Consider making a table with the menu settings. Something like this ...
  Table: ocelot.actions
  Window  Menu_position Menu_item_position Menu_name Menu_item_name Name  Caption/Hint    Menu   Key   Toolbar Action
  ------  ------------- ------------------ --------- -------------- ----  ------------    ----   ---   ------- ------
  Main                1                  1 File      E&xit                                                     exit
  Main    1             1                  File      Exitrun   Execute         Edit   &E    lightning-bolt
  This is creation -- assume there are no existing menus.
  There could also be alter -- wipe existing actions as part of user customization, then call this.
     We do not want users to get rid of "About" box that shows copyrights etc.
  ?? How do I get rid of a QAction?
    Also set up more with setShortcuts(), setStatusTip(), ...
    Also add action to toolbox
     This would addMenu() multiple times for the same menu, so I use a different name each time.
    I should show what the shortcuts are; unfortunately for most actions the shortcut differs according to platform
    Maybe instead of "Execute ctrl+E" I should be using "Run ctrl+R"
    Sure, I tried using & for various menu items e.g. E&xit -- but then Alt+x did not work!
  Todo: With Ubuntu 14.04 the ^O and ^E and ^Q keys didn't work, find out what goes on.
*/
void MainWindow::create_menu()
{
  menu_file= ui->menuBar->addMenu(tr("File"));
  /* Todo: consider adding fileMenu = new QMenu(tr("&File"), this); -*/
  menu_file_action_connect= menu_file->addAction(tr("Connect"));
  connect(menu_file_action_connect, SIGNAL(triggered()), this, SLOT(action_connect()));
  menu_file_action_connect->setShortcut(QKeySequence::Open); /* Todo: think of a better key sequence than ctrl-o */

  menu_file_action_exit= menu_file->addAction(tr("Exit"));
  connect(menu_file_action_exit, SIGNAL(triggered()), this, SLOT(action_exit()));
  menu_file_action_exit->setShortcut(QKeySequence::Quit);

  /*
    Most Edit menu actions don't require slots from me -- cut() etc. already exist for class QTextEdit.
    I just need to ensure they're on the menu.
  */
  menu_edit= ui->menuBar->addMenu(tr("Edit"));
  menu_edit_action_undo= menu_edit->addAction(tr("Undo"));
  connect(menu_edit_action_undo, SIGNAL(triggered()), this, SLOT(action_undo()));
  menu_edit_action_undo->setShortcut(QKeySequence::Undo);
  menu_edit_action_redo= menu_edit->addAction(tr("Redo"));
  connect(menu_edit_action_redo, SIGNAL(triggered()), statement_edit_widget, SLOT(redo()));
  menu_edit_action_redo->setShortcut(QKeySequence::Redo);
  menu_edit->addSeparator();
  menu_edit_action_cut= menu_edit->addAction(tr("Cut"));
  QTextEdit::connect(menu_edit_action_cut, SIGNAL(triggered()), statement_edit_widget, SLOT(cut()));
  menu_edit_action_cut->setShortcut(QKeySequence::Cut);
  menu_edit_action_copy= menu_edit->addAction(tr("Copy"));
  connect(menu_edit_action_copy, SIGNAL(triggered()), statement_edit_widget, SLOT(copy()));
  menu_edit_action_copy->setShortcut(QKeySequence::Copy);
  menu_edit_action_paste= menu_edit->addAction(tr("Paste"));
  connect(menu_edit_action_paste, SIGNAL(triggered()), statement_edit_widget, SLOT(paste()));
  menu_edit_action_paste->setShortcut(QKeySequence::Paste);
  menu_edit->addSeparator();
  menu_edit_action_select_all= menu_edit->addAction(tr("Select All"));
  connect(menu_edit_action_select_all, SIGNAL(triggered()), statement_edit_widget, SLOT(selectAll()));
  menu_edit_action_select_all->setShortcut(QKeySequence::SelectAll);

  menu_edit_action_cut->setEnabled(false);
  menu_edit_action_copy->setEnabled(false);
  menu_edit_action_undo->setEnabled(statement_edit_widget->document()->isUndoAvailable());
  menu_edit_action_redo->setEnabled(statement_edit_widget->document()->isRedoAvailable());

  connect(statement_edit_widget, SIGNAL(copyAvailable(bool)),menu_edit_action_cut, SLOT(setEnabled(bool)));
  connect(statement_edit_widget, SIGNAL(copyAvailable(bool)),menu_edit_action_copy, SLOT(setEnabled(bool)));

  connect(statement_edit_widget->document(), SIGNAL(undoAvailable(bool)),menu_edit_action_undo, SLOT(setEnabled(bool)));
  connect(statement_edit_widget->document(), SIGNAL(redoAvailable(bool)),menu_edit_action_redo, SLOT(setEnabled(bool)));

  menu_edit_action_history_markup_previous= menu_edit->addAction(tr("Previous statement"));
  connect(menu_edit_action_history_markup_previous, SIGNAL(triggered()), this, SLOT(history_markup_previous()));
  menu_edit_action_history_markup_previous->setShortcut(QKeySequence(tr("Ctrl+P")));

  menu_edit_action_history_markup_next= menu_edit->addAction(tr("Next statement"));
  connect(menu_edit_action_history_markup_next, SIGNAL(triggered()), this, SLOT(history_markup_next()));
  menu_edit_action_history_markup_next->setShortcut(QKeySequence(tr("Ctrl+N")));

  menu_run= ui->menuBar->addMenu(tr("Run"));
  menu_run_action_execute= menu_run->addAction(tr("Execute"));
  connect(menu_run_action_execute, SIGNAL(triggered()), this, SLOT(action_execute()));
  menu_run_action_execute->setShortcut(QKeySequence(tr("Ctrl+E")));
  menu_run_action_kill= menu_run->addAction(tr("Kill"));
  connect(menu_run_action_kill, SIGNAL(triggered()), this, SLOT(action_kill()));
  menu_run_action_kill->setShortcut(QKeySequence(tr("Ctrl+C")));
  menu_run_action_kill->setEnabled(false);

  menu_settings= ui->menuBar->addMenu(tr("Settings"));
  menu_settings_action_statement= menu_settings->addAction(tr("Statement Widget: Colors and Fonts"));
  menu_settings_action_grid= menu_settings->addAction(tr("Grid Widget: Colors and Fonts"));
  menu_settings_action_history= menu_settings->addAction(tr("History Widget: Colors and Fonts"));
  menu_settings_action_main= menu_settings->addAction(tr("Main Window: Colors and Fonts"));
  connect(menu_settings_action_statement, SIGNAL(triggered()), this, SLOT(action_statement()));
  connect(menu_settings_action_grid, SIGNAL(triggered()), this, SLOT(action_grid()));
  connect(menu_settings_action_history, SIGNAL(triggered()), this, SLOT(action_history()));
  connect(menu_settings_action_main, SIGNAL(triggered()), this, SLOT(action_main()));

  menu_options= ui->menuBar->addMenu(tr("Options"));


  menu_options_action_option_display_blob_as_image= menu_options->addAction(tr("display BLOB as image"));
  menu_options_action_option_display_blob_as_image->setCheckable(true);
  menu_options_action_option_display_blob_as_image->setChecked(ocelot_display_blob_as_image);
  connect(menu_options_action_option_display_blob_as_image, SIGNAL(triggered(bool)), this, SLOT(action_option_display_blob_as_image(bool)));

#ifdef DEBUGGER
  menu_debug= ui->menuBar->addMenu(tr("Debug"));

//  menu_debug_action_install= menu_debug->addAction(tr("Install"));
//  connect(menu_debug_action_install, SIGNAL(triggered()), this, SLOT(action_debug_install()));
//  menu_debug_action_install->setShortcut(QKeySequence(tr("Alt+A")));

//  menu_debug_action_setup= menu_debug->addAction(tr("Setup"));
//  connect(menu_debug_action_setup, SIGNAL(triggered()), this, SLOT(action_debug_setup()));
//  menu_debug_action_setup->setShortcut(QKeySequence(tr("Alt+5")));

//  menu_debug_action_debug= menu_debug->addAction(tr("Debug"));
//  connect(menu_debug_action_debug, SIGNAL(triggered()), this, SLOT(action_debug_debug()));
//  menu_debug_action_debug->setShortcut(QKeySequence(tr("Alt+3")));

  menu_debug_action_breakpoint= menu_debug->addAction(tr("Breakpoint"));
  connect(menu_debug_action_breakpoint, SIGNAL(triggered()), this, SLOT(action_debug_breakpoint()));
  menu_debug_action_breakpoint->setShortcut(QKeySequence(tr("Alt+1")));

  menu_debug_action_continue= menu_debug->addAction(tr("Continue"));
  connect(menu_debug_action_continue, SIGNAL(triggered()), this, SLOT(action_debug_continue()));
  menu_debug_action_continue->setShortcut(QKeySequence(tr("Alt+2")));

//  menu_debug_action_leave= menu_debug->addAction(tr("Leave"));
//  connect(menu_debug_action_leave, SIGNAL(triggered()), this, SLOT(action_debug_leave()));
//  menu_debug_action_leave->setShortcut(QKeySequence(tr("Alt+B")));

  menu_debug_action_next= menu_debug->addAction(tr("Next"));
  connect(menu_debug_action_next, SIGNAL(triggered()), this, SLOT(action_debug_next()));
  menu_debug_action_next->setShortcut(QKeySequence(tr("Alt+3")));

//  menu_debug_action_skip= menu_debug->addAction(tr("Skip"));
//  connect(menu_debug_action_skip, SIGNAL(triggered()), this, SLOT(action_debug_skip()));
//  menu_debug_action_skip->setShortcut(QKeySequence(tr("Alt+4")));

  menu_debug_action_step= menu_debug->addAction(tr("Step"));
  connect(menu_debug_action_step, SIGNAL(triggered()), this, SLOT(action_debug_step()));
  menu_debug_action_step->setShortcut(QKeySequence(tr("Alt+5")));

  menu_debug_action_clear= menu_debug->addAction(tr("Clear"));
  connect(menu_debug_action_clear, SIGNAL(triggered()), this, SLOT(action_debug_clear()));
  menu_debug_action_clear->setShortcut(QKeySequence(tr("Alt+6")));

//  menu_debug_action_delete= menu_debug->addAction(tr("Delete"));
//  connect(menu_debug_action_delete, SIGNAL(triggered()), this, SLOT(action_debug_delete()));
//  menu_debug_action_delete->setShortcut(QKeySequence(tr("Alt+G")));

  menu_debug_action_exit= menu_debug->addAction(tr("Exit"));
  connect(menu_debug_action_exit, SIGNAL(triggered()), this, SLOT(action_debug_exit()));
  menu_debug_action_exit->setShortcut(QKeySequence(tr("Alt+7")));

  menu_debug_action_information= menu_debug->addAction(tr("Information"));
  connect(menu_debug_action_information, SIGNAL(triggered()), this, SLOT(action_debug_information()));
  menu_debug_action_information->setShortcut(QKeySequence(tr("Alt+8")));

  menu_debug_action_refresh_server_variables= menu_debug->addAction(tr("Refresh server variables"));
  connect(menu_debug_action_refresh_server_variables, SIGNAL(triggered()), this, SLOT(action_debug_refresh_server_variables()));
  menu_debug_action_refresh_server_variables->setShortcut(QKeySequence(tr("Alt+9")));

  menu_debug_action_refresh_user_variables= menu_debug->addAction(tr("Refresh user variables"));
  connect(menu_debug_action_refresh_user_variables, SIGNAL(triggered()), this, SLOT(action_debug_refresh_user_variables()));
  menu_debug_action_refresh_user_variables->setShortcut(QKeySequence(tr("Alt+0")));

  menu_debug_action_refresh_variables= menu_debug->addAction(tr("Refresh variables"));
  connect(menu_debug_action_refresh_variables, SIGNAL(triggered()), this, SLOT(action_debug_refresh_variables()));
  menu_debug_action_refresh_variables->setShortcut(QKeySequence(tr("Alt+A")));

  menu_debug_action_refresh_call_stack= menu_debug->addAction(tr("Refresh call_stack"));
  connect(menu_debug_action_refresh_call_stack, SIGNAL(triggered()), this, SLOT(action_debug_refresh_call_stack()));
  menu_debug_action_refresh_call_stack->setShortcut(QKeySequence(tr("Alt+B")));

  debug_menu_enable_or_disable(TOKEN_KEYWORD_BEGIN); /* Disable most of debug menu */

#endif

  menu_help= ui->menuBar->addMenu(tr("Help"));
  menu_help_action_about= menu_help->addAction(tr("About"));
  connect(menu_help_action_about, SIGNAL(triggered()), this, SLOT(action_about()));
  menu_help_action_the_manual= menu_help->addAction(tr("The Manual"));
  connect(menu_help_action_the_manual, SIGNAL(triggered()), this, SLOT(action_the_manual()));

  /* Qt says I should also do "addSeparator" if Motif style. Harmless. */
  ui->menuBar->addSeparator();
  /* exitAction->setPriority(QAction::LowPriority); */
}


/*
  ACTIONS
  ! All action_ functions must be in the "public slots" area of ocelotgui.h
*/

/*
  action_statement_edit_widget_text_changed() is a slot.
  Actually the connect() was for statement_edit_widget->document()'s contentsChanged(),
  rather than statement_edit_widget's textChanged(), but I don't see any difference.
*/
void MainWindow::action_statement_edit_widget_text_changed()
{
  QString text;
  int i;
  // int j;
  int pos;
  QTextCharFormat format_of_current_token;

  /*
    When insertText happens later in this routine, it causes a signal
    and we get action_statement_edit_widget_text_changed() again.
    It would be infinite. Doubtless there are good ways to avoid looping.
    I don't know them. So I'll exit if a global flag is on, then set it.
  */
  /* Todo: check whether "wasModified" would have been a better slot. */
  if (statement_edit_widget_text_changed_flag != 0) return;
  statement_edit_widget_text_changed_flag= 1;

  /* Syntax highlighting */
  text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */

  /* Todo: avoid total tokenize() + tokens_to_keywords() if user is just adding at end */

  tokenize(text.data(),
           text.size(),
           &main_token_lengths, &main_token_offsets, MAX_TOKENS,(QChar*)"33333", 1, ocelot_delimiter_str, 1);

  tokens_to_keywords(text);

  /* This "sets" the colour, it does not "merge" it. */
  /* Do not try to set underlines, they won't go away. */
  QTextDocument *pDoc= statement_edit_widget->document();
  QTextCursor cur(pDoc);

  /* cur.select (QTextCursor::Document); */ /* desperate attempt to fix so undo/redo is not destroyed ... does not work */

  QTextCharFormat format_of_literal;
  format_of_literal.setForeground(QColor(ocelot_statement_highlight_literal_color));
  QTextCharFormat format_of_identifier;
  format_of_identifier.setForeground(QColor(ocelot_statement_highlight_identifier_color));
  QTextCharFormat format_of_comment;
  format_of_comment.setForeground(QColor(ocelot_statement_highlight_comment_color));
  QTextCharFormat format_of_operator;
  format_of_operator.setForeground(QColor(ocelot_statement_highlight_operator_color));
  QTextCharFormat format_of_reserved_word;
  format_of_reserved_word.setForeground(QColor(ocelot_statement_highlight_reserved_color));
  QTextCharFormat format_of_other;
  format_of_other.setForeground(QColor(ocelot_statement_color));

  pos= 0;
  /* cur.setPosition(pos, QTextCursor::KeepAnchor); */
  cur.beginEditBlock();                                     /* ought to affect undo/redo stack? */

  for (i= 0; main_token_lengths[i] != 0; ++i)
  {
    /* Todo: consider: If we've gone over some white space, let it be normal format */
    /* Todo: check if this is working. An earlier test suggested that at least with underline it isn't working. */
    for (; pos < main_token_offsets[i]; ++pos)
    {
      cur.setPosition(pos, QTextCursor::MoveAnchor);
      cur.setPosition(pos, QTextCursor::KeepAnchor);
      cur.setCharFormat(format_of_other);
      /* cur.clearSelection(); */
    }
    int t= main_token_types[i];
    if (t == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE) format_of_current_token= format_of_literal;
    if (t == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE) format_of_current_token= format_of_literal;
    if (t == TOKEN_TYPE_LITERAL_WITH_DIGIT) format_of_current_token= format_of_literal;
    if (t == TOKEN_TYPE_LITERAL_WITH_BRACE) format_of_current_token= format_of_literal;
    if (t == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK) format_of_current_token= format_of_identifier;
    if (t == TOKEN_TYPE_IDENTIFIER_WITH_AT) format_of_current_token= format_of_identifier;
    if (t == TOKEN_TYPE_COMMENT_WITH_SLASH) format_of_current_token= format_of_comment;
    if (t == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE) format_of_current_token= format_of_comment;
    if (t == TOKEN_TYPE_COMMENT_WITH_MINUS) format_of_current_token= format_of_comment;
    if (t == TOKEN_TYPE_OPERATOR) format_of_current_token= format_of_operator;
    if (t >= TOKEN_KEYWORDS_START) format_of_current_token= format_of_reserved_word;
    if (t == TOKEN_TYPE_OTHER) format_of_current_token= format_of_other;

    cur.setPosition(pos, QTextCursor::MoveAnchor);
    cur.setPosition(pos+main_token_lengths[i], QTextCursor::KeepAnchor);
    cur.setCharFormat(format_of_current_token);
    pos+= main_token_lengths[i];
  }
  cur.endEditBlock();

  /* Todo: consider what to do about trailing whitespace. */

  widget_sizer(); /* Perhaps adjust relative sizes of the main widgets. */

  statement_edit_widget_text_changed_flag= 0;
}


/*
  For menu item "connect" we said connect(...SLOT(action_connect())));
  By default this is on and associated with File|Connect menu item.
  Actually connect will be attempted depending on defaults and command-line parameters.
*/
void MainWindow::action_connect()
{
  action_connect_once(tr("File|Connect"));
}


/*
  Todo:
  If user types OK and there's an error, repeat the dialog box with a new message e.g. "Connect failed ...".
  This is called from program-start!
  This should put "CONNECT" in the statement widget and cause its execution, so it shows up on the history widget.
*/

void MainWindow::action_connect_once(QString message)
{
  int column_count;
  QString *row_form_label;
  int *row_form_type;
  int *row_form_is_password;
  QString *row_form_data;
  QString *row_form_width;
  QString row_form_title;
  QString row_form_message;
  int i;
  Row_form_box *co;

  row_form_label= 0;
  row_form_type= 0;
  row_form_is_password= 0;
  row_form_data= 0;
  row_form_width= 0;
  column_count= 80; /* If you add or remove items, you have to change this */
  row_form_label= new QString[column_count];
  row_form_type= new int[column_count];
  row_form_is_password= new int[column_count];
  row_form_data= new QString[column_count];
  row_form_width= new QString[column_count];

  row_form_label[i=0]= "host"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_host; row_form_width[i]= 80;
  row_form_label[++i]= "port"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_port); row_form_width[i]= 4;
  row_form_label[++i]= "user"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_user; row_form_width[i]= 80;
  row_form_label[++i]= "database"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_database; row_form_width[i]= 80;
  row_form_label[++i]= "socket"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_unix_socket; row_form_width[i]= 80;
  row_form_label[++i]= "password"; row_form_type[i]= 0; row_form_is_password[i]= 1; row_form_data[i]= ocelot_password; row_form_width[i]= 80;
  row_form_label[++i]= "protocol"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_protocol; row_form_width[i]= 80;
  row_form_label[++i]= "init_command"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_init_command; row_form_width[i]= 80;
  row_form_label[++i]= "auto_rehash"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_auto_rehash); row_form_width[i]= 5;
  row_form_label[++i]= "auto_vertical_output"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_auto_vertical_output); row_form_width[i]= 5;
  row_form_label[++i]= "batch"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_batch); row_form_width[i]= 5;
  row_form_label[++i]= "binary_mode"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_binary_mode); row_form_width[i]= 5;
  row_form_label[++i]= "bind_address"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_bind_address; row_form_width[i]= 5;
  row_form_label[++i]= "character_sets_dir"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_set_charset_dir; row_form_width[i]= 5;
  row_form_label[++i]= "column_names"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_result_grid_column_names); row_form_width[i]= 5;
  row_form_label[++i]= "column_type_info"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_column_type_info); row_form_width[i]= 5;
  row_form_label[++i]= "comments"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_comments); row_form_width[i]= 5;
  row_form_label[++i]= "compress"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_opt_compress); row_form_width[i]= 5;
  row_form_label[++i]= "connect_expired_password"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_connect_expired_password); row_form_width[i]= 5;
  row_form_label[++i]= "connect_tmeout"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_opt_connect_timeout); row_form_width[i]= 5;
  row_form_label[++i]= "debug"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_debug; row_form_width[i]= 5;
  row_form_label[++i]= "debug_check"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_debug_check); row_form_width[i]= 5;
  row_form_label[++i]= "debug_info"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_debug_info); row_form_width[i]= 5;
  row_form_label[++i]= "default_auth"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_default_auth; row_form_width[i]= 5;
  row_form_label[++i]= "default_character_set"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_set_charset_name; row_form_width[i]= 5;
  row_form_label[++i]= "defaults_extra_file"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_defaults_extra_file; row_form_width[i]= 5;
  row_form_label[++i]= "defaults_file"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_defaults_file; row_form_width[i]= 5;
  row_form_label[++i]= "defaults_group_suffix"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_read_default_group; row_form_width[i]= 5;
  row_form_label[++i]= "delimiter"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_delimiter_str; row_form_width[i]= 5;
  row_form_label[++i]= "enable_cleartext_plugin"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_enable_cleartext_plugin); row_form_width[i]= 5;
  row_form_label[++i]= "execute"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_execute; row_form_width[i]= 5;
  row_form_label[++i]= "force"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_force); row_form_width[i]= 5;
  row_form_label[++i]= "help"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0;  row_form_data[i]= QString::number(ocelot_help); row_form_width[i]= 5;
  row_form_label[++i]= "histignore"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_histignore); row_form_width[i]= 5;
  row_form_label[++i]= "html"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_html); row_form_width[i]= 5;
  row_form_label[++i]= "ignore_spaces"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_ignore_spaces); row_form_width[i]= 5;
  row_form_label[++i]= "line_numbers"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_line_numbers); row_form_width[i]= 5;
  row_form_label[++i]= "local_infile"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_opt_local_infile); row_form_width[i]= 5;
  row_form_label[++i]= "login_path"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_login_path; row_form_width[i]= 5;
  row_form_label[++i]= "max_allowed_packet"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(max_allowed_packet); row_form_width[i]= 5;
  row_form_label[++i]= "max_join_size"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_max_join_size); row_form_width[i]= 5;
  row_form_label[++i]= "named_commands"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_named_commands); row_form_width[i]= 5;
  row_form_label[++i]= "net_buffer_length"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_net_buffer_length); row_form_width[i]= 5;
  row_form_label[++i]= "no_beep"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_no_beep); row_form_width[i]= 5;
  row_form_label[++i]= "no_defaults"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_no_defaults); row_form_width[i]= 5;
  row_form_label[++i]= "one_database"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_one_database); row_form_width[i]= 5;
  row_form_label[++i]= "pager"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_pager; row_form_width[i]= 5;
  row_form_label[++i]= "pipe"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_pipe); row_form_width[i]= 5;
  row_form_label[++i]= "plugin_dir"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_plugin_dir; row_form_width[i]= 5;
  row_form_label[++i]= "print_defaults"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_print_defaults); row_form_width[i]= 5;
  row_form_label[++i]= "prompt"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_prompt; row_form_width[i]= 5;
  row_form_label[++i]= "quick"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_quick); row_form_width[i]= 5;
  row_form_label[++i]= "raw"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_raw); row_form_width[i]= 5;
  row_form_label[++i]= "reconnect"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_opt_reconnect); row_form_width[i]= 5;
  row_form_label[++i]= "safe_updates"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_safe_updates); row_form_width[i]= 5;
  row_form_label[++i]= "secure_auth"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_secure_auth); row_form_width[i]= 5;
  row_form_label[++i]= "select_limit"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_select_limit); row_form_width[i]= 5;
  row_form_label[++i]= "server_public_key"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_server_public_key; row_form_width[i]= 5;
  row_form_label[++i]= "shared_memory_base_name"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_shared_memory_base_name; row_form_width[i]= 5;
  /* It used to crash if I said number(ocelot_history_includes_warnings). Problem has disappeared. */
  row_form_label[++i]= "show_warnings"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_history_includes_warnings); row_form_width[i]= 5;
  row_form_label[++i]= "sigint_ignore"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_sigint_ignore); row_form_width[i]= 5;
  row_form_label[++i]= "silent"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_silent); row_form_width[i]= 5;
  row_form_label[++i]= "ssl"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_ca"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_ca; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_capath"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_capath; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_cert"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_cert; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_cipher"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_cipher; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_crl"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_crl; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_crlpath"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_crlpath; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_key"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_opt_ssl_key; row_form_width[i]= 5;
  row_form_label[++i]= "ssl_verify_server_cert"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_opt_ssl_verify_server_cert); row_form_width[i]= 5;
  row_form_label[++i]= "syslog"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_syslog); row_form_width[i]= 5;
  row_form_label[++i]= "table"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_table); row_form_width[i]= 5;
  row_form_label[++i]= "tee"; row_form_type[i]= 0; row_form_is_password[i]= 0; row_form_data[i]= ocelot_history_tee_file_name; row_form_width[i]= 5;
  row_form_label[++i]= "unbuffered"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_unbuffered); row_form_width[i]= 5;
  row_form_label[++i]= "verbose"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_verbose); row_form_width[i]= 5;
  row_form_label[++i]= "version"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_version); row_form_width[i]= 5;
  row_form_label[++i]= "vertical"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_result_grid_vertical); row_form_width[i]= 5;
  row_form_label[++i]= "wait"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_wait); row_form_width[i]= 5;
  row_form_label[++i]= "xml"; row_form_type[i]= NUM_FLAG; row_form_is_password[i]= 0; row_form_data[i]= QString::number(ocelot_xml); row_form_width[i]= 5;

  row_form_title= tr("Connection Dialog Box");
  row_form_message= message;

  co= new Row_form_box(column_count, row_form_label,
//                                     row_form_type,
                                     row_form_is_password, row_form_data,
//                                     row_form_width,
                                     row_form_title, row_form_message, this);
  co->exec();

  if (co->is_ok == 1)
  {
    ocelot_host= row_form_data[0].trimmed();
    ocelot_port= row_form_data[1].trimmed().toInt();
    ocelot_user= row_form_data[2].trimmed();
    ocelot_database= row_form_data[3].trimmed();
    ocelot_unix_socket= row_form_data[4].trimmed();
    ocelot_password= row_form_data[5].trimmed();
    ocelot_protocol= row_form_data[6].trimmed(); ocelot_protocol_as_int= get_ocelot_protocol_as_int(ocelot_protocol);
    ocelot_init_command= row_form_data[7].trimmed();
    ocelot_auto_rehash= row_form_data[8].trimmed().toInt();

    i= 9;
    ocelot_auto_vertical_output= row_form_data[i++].trimmed().toInt();
    ocelot_batch= row_form_data[i++].trimmed().toInt();
    ocelot_binary_mode= row_form_data[i++].trimmed().toInt();
    ocelot_bind_address= row_form_data[i++].trimmed();
    ocelot_set_charset_dir= row_form_data[i++].trimmed(); /* "character_sets_dir" */
    ocelot_result_grid_column_names= row_form_data[i++].trimmed().toInt();
    ocelot_column_type_info= row_form_data[i++].trimmed().toInt();
    ocelot_comments= row_form_data[i++].trimmed().toInt();
    ocelot_opt_compress= row_form_data[i++].trimmed().toInt();
    ocelot_connect_expired_password= row_form_data[i++].trimmed().toInt();
    ocelot_opt_connect_timeout= row_form_data[i++].trimmed().toInt();
    ocelot_debug= row_form_data[i++].trimmed();
    ocelot_debug_check= row_form_data[i++].trimmed().toInt();
    ocelot_debug_info= row_form_data[i++].trimmed().toInt();
    ocelot_default_auth= row_form_data[i++].trimmed();
    ocelot_set_charset_name= row_form_data[i++].trimmed(); /* "default_character_set" */
    ocelot_defaults_extra_file= row_form_data[i++].trimmed();
    ocelot_defaults_file= row_form_data[i++].trimmed();
    ocelot_read_default_group= row_form_data[i++].trimmed();
    ocelot_delimiter_str= row_form_data[i++].trimmed();
    ocelot_enable_cleartext_plugin= row_form_data[i++].trimmed().toInt();
    ocelot_execute= row_form_data[i++].trimmed();
    ocelot_force= row_form_data[i++].trimmed().toInt();
    ocelot_help= row_form_data[i++].trimmed().toInt();
    ocelot_histignore= row_form_data[i++].trimmed().toInt();
    ocelot_html= row_form_data[i++].trimmed().toInt();
    ocelot_ignore_spaces= row_form_data[i++].trimmed().toInt();
    ocelot_line_numbers= row_form_data[i++].trimmed().toInt();
    ocelot_opt_local_infile= row_form_data[i++].trimmed().toInt();
    ocelot_login_path= row_form_data[i++].trimmed();
    max_allowed_packet= row_form_data[i++].trimmed().toInt();
    ocelot_max_join_size= row_form_data[i++].trimmed().toInt();
    ocelot_named_commands= row_form_data[i++].trimmed().toInt();
    ocelot_net_buffer_length= row_form_data[i++].trimmed().toInt();
    ocelot_no_beep= row_form_data[i++].trimmed().toInt();
    ocelot_no_defaults= row_form_data[i++].trimmed().toInt();
    ocelot_one_database= row_form_data[i++].trimmed().toInt();
    ocelot_pager= row_form_data[i++].trimmed();
    ocelot_pipe= row_form_data[i++].trimmed().toInt();
    ocelot_plugin_dir= row_form_data[i++].trimmed();
    ocelot_print_defaults= row_form_data[i++].trimmed().toInt();
    ocelot_prompt= row_form_data[i++].trimmed();
    ocelot_quick= row_form_data[i++].trimmed().toInt();
    ocelot_raw= row_form_data[i++].trimmed().toInt();
    ocelot_opt_reconnect= row_form_data[i++].trimmed().toInt();
    ocelot_safe_updates= row_form_data[i++].trimmed().toInt();
    ocelot_secure_auth= row_form_data[i++].trimmed().toInt();
    ocelot_select_limit= row_form_data[i++].trimmed().toInt();
    ocelot_server_public_key= row_form_data[i++].trimmed();
    ocelot_shared_memory_base_name= row_form_data[i++].trimmed();
    ocelot_history_includes_warnings= row_form_data[i++].trimmed().toInt();
    ocelot_sigint_ignore= row_form_data[i++].trimmed().toInt();
    ocelot_silent= row_form_data[i++].trimmed().toInt();
    ocelot_opt_ssl= row_form_data[i++].trimmed();
    ocelot_opt_ssl_ca= row_form_data[i++].trimmed();
    ocelot_opt_ssl_capath= row_form_data[i++].trimmed();
    ocelot_opt_ssl_cert= row_form_data[i++].trimmed();
    ocelot_opt_ssl_cipher= row_form_data[i++].trimmed();
    ocelot_opt_ssl_crl= row_form_data[i++].trimmed();
    ocelot_opt_ssl_crlpath= row_form_data[i++].trimmed();
    ocelot_opt_ssl_key= row_form_data[i++].trimmed();
    ocelot_opt_ssl_verify_server_cert= row_form_data[i++].trimmed().toInt();
    ocelot_syslog= row_form_data[i++].trimmed().toInt();
    ocelot_table= row_form_data[i++].trimmed().toInt();
    ocelot_history_tee_file_name= row_form_data[i++].trimmed();
    ocelot_unbuffered= row_form_data[i++].trimmed().toInt();
    ocelot_verbose= row_form_data[i++].trimmed().toInt();
    ocelot_version= row_form_data[i++].trimmed().toInt();
    ocelot_result_grid_vertical= row_form_data[i++].trimmed().toInt();
    ocelot_wait= row_form_data[i++].trimmed().toInt();
    ocelot_xml= row_form_data[i++].trimmed().toInt();

    /* This should ensure that a record goes to the history widget */
    /* Todo: clear statement_edit_widget first */
    statement_edit_widget->insertPlainText("CONNECT");
    action_execute();
    if (ocelot_init_command > "")
    {
      statement_edit_widget->insertPlainText(ocelot_init_command);
      action_execute();
    }
  }
  delete(co);

  if (row_form_width != 0) delete [] row_form_width;
  if (row_form_data != 0) delete [] row_form_data;
  if (row_form_is_password != 0)  delete [] row_form_is_password;
  if (row_form_type != 0) delete [] row_form_type;
  if (row_form_label != 0) delete [] row_form_label;
}


/*
  For menu item "exit" we said connect(...SLOT(action_exit())));
  By default this is on and associated with File|Exit menu item.
  Stop the program.
*/
void MainWindow::action_exit()
{
#ifdef DEBUGGER
  /* Get rid of debuggee if it's still around. */
  /* Todo: this might not be enough. Maybe you should be intercepting the "close window" event. */
  if (menu_debug_action_exit->isEnabled() == true) action_debug_exit();
#endif
  close();
}





/* menu item = Options|display BLOB as image */
void MainWindow::action_option_display_blob_as_image(bool checked)
{
  ocelot_display_blob_as_image= checked;
}


/* Todo: consider adding   //printf(qVersion()); */
void MainWindow::action_about()
{
  QMessageBox msgBox;

  msgBox.setWindowTitle(tr("Help | About"));
  msgBox.setText("<b>ocelotgui -- Ocelot Graphical User Interface</b><br>Copyright (c) 2014 by Ocelot Computer Services Inc.<br>\
This program is free software: you can redistribute it and/or modify \
it under the terms of the GNU General Public License as published by \
the Free Software Foundation, either version 3 of the License, or \
(at your option) any later version.<br>\
<br>\
This program is distributed in the hope that it will be useful, \
but WITHOUT ANY WARRANTY; without even the implied warranty of \
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the \
GNU General Public License for more details.<br>\
<br>\
You should have received a copy of the GNU General Public License \
along with this program.  If not, see &lt;http://www.gnu.org/licenses/&gt;.");
    msgBox.exec();
}


/*
  the_manual_widget will be shown when user clicks Help | The Manual.
  It does not inherit the fonts+colors settings of the main window.
  The text is copied from manual.htm; all lines in manual.htm end
  with two spaces so there is a blanket replacement of all "  " to "  \".
*/
void MainWindow::create_the_manual_widget()
{
  the_manual_text_edit= new QTextEdit();
  the_manual_pushbutton= new QPushButton(tr("Close"));
  connect(the_manual_pushbutton, SIGNAL(clicked()), this, SLOT(action_the_manual_close()));
  the_manual_layout= new QVBoxLayout();
  the_manual_layout->addWidget(the_manual_text_edit);
  the_manual_layout->addWidget(the_manual_pushbutton);
  the_manual_widget= new QWidget();
  the_manual_widget->setLayout(the_manual_layout);

  the_manual_widget->setWindowTitle(tr("The Manual"));
  the_manual_widget->setMinimumWidth(250); /* should be equal to width of title, at least */

  the_manual_text_edit->setText("\
<BR><h1>The ocelotgui user manual</h1>  \
<BR>  \
<BR>Version 0.4.0, April 2 2015  \
<BR>  \
<BR>  \
<BR>Copyright (c) 2014 by Ocelot Computer Services Inc. All rights reserved.  \
<BR>  \
<BR>This program is free software; you can redistribute it and/or modify  \
<BR>it under the terms of the GNU General Public License as published by  \
<BR>the Free Software Foundation; version 2 of the License.  \
<BR>  \
<BR>This program is distributed in the hope that it will be useful,  \
<BR>but WITHOUT ANY WARRANTY; without even the implied warranty of  \
<BR>MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the  \
<BR>GNU General Public License for more details.  \
<BR>  \
<BR>You should have received a copy of the GNU General Public License  \
<BR>along with this program; if not, write to the Free Software  \
<BR>Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA  \
<BR>  \
<BR>  \
<BR><h2>Executive Summary</h2>  \
<BR>  \
<BR>The ocelotgui application, more fully  \
The Ocelot Graphical User Interface', allows users to connect to  \
a MySQL or MariaDB DBMS server, enter SQL statements, and receive results.  \
Some of its features are: syntax highlighting, user-settable colors  \
and fonts for each part of the screen, and result-set displays  \
with multi-line rows and resizable columns.  \
NEW: And a debugger, see section 'Debugger'.  \
<BR>  \
<BR><h2>The company, the product, and the status</h2>  \
<BR>  \
<BR>Ocelot Computer Services is a Canadian company  \
which has specialized in database products for thirty years.  \
Its two employees both worked for MySQL AB and  \
Sun Microsystems and Oracle between 2003 and 2011.  \
<BR>  \
<BR>The ocelotgui program is a front end which connects to MySQL (tm) or MariaDB (tm).  \
In some ways it is like the basic mysql client program,  \
with added GUI features: full-screen editing, syntax  \
highlighting, tabular display, customized fonts and colors.  \
It differs from some other front-end GUI products because  \
it is open source (GPL), is written in C++, and makes use  \
of the Qt multi-platform widget library.  \
<BR>  \
<BR>The product status is: alpha. It has been known to work as described in  \
this manual on several Linux distros. But testing has been short  \
and the 'todo' list is long. We are making an early release for  \
tire kickers who will want to confirm what the manual describes,  \
or want to see the source code and do their own builds.  \
<BR>  \
<BR><h2>Downloading, installing, and building</h2>  \
<BR>  \
<BR>To download the product go to  \
https://github.com/ocelot-inc/ocelotgui.  \
Instructions for installation will be in the README.txt file.  \
This location may change, or alternate locations may appear.  \
If so there will either be an announcement on github or on ocelot.ca.  \
<BR>  \
<BR>The package contains source code, and  \
two executable files, one for use with Qt version 4 and the  \
other for use with Qt version 5.  \
The executables are built dynamically.  \
They look for pre-installed copies of Qt libraries and of the  \
MySQL client library -- see the README.txt file about these.  \
The executables were built with Ubuntu 12.04 and may not work  \
well on other Linux distros -- if that is the case, consult  \
the README.txt and the source code to see how to build from  \
source.  \
<BR><h2>Starting</h2>  \
<BR>  \
<BR>There must be an instance of MySQL or MariaDB running somewhere.  \
<BR>  \
<BR>If connection is possible with the mysql client and does not require  \
unusual options, then connection is possible with ocelotgui. If there is  \
a my.cnf file, ocelotgui will read it, just as the mysql client would.  \
If there are connection-related options on the command line, ocelotgui  \
will accept them just as the mysql client would. Therefore the typical  \
way to start the program is to say  \
ocelotgui [--option [, option...]]  \
<BR>  \
<BR>If a password is required but not supplied, or if the initial  \
attempt to connect fails, a dialog box will appear.  \
Fill in or correct the fields on the dialog box and click OK.  \
If the connection still fails, then ocelotgui will still come up,  \
but only non-DBMS tasks such as screen customizing will be possible.  \
<BR>  \
<BR>In any case, an initial screen will appear. After some activity has  \
taken place, the screen will have four parts, from top to bottom:  \
<BR>menu  \
<BR>history widget, where retired statements and diagnostics end up  \
<BR>results widget, where SELECT result sets appear  \
<BR>statement widget, where users can type in instructions.  \
<BR>Initially, though, only the menu and statement widget will appear.  \
<BR>  \
<BR>Again, this should be reminiscent of the way the mysql client works:  \
statements are typed at the bottom of the screen, and appear to  \
scroll off the top after they are executed, with results in the middle.  \
<BR>  \
<BR><h2>Statement widget</h2>  \
<BR>  \
<BR>The statement widget is an editable multi-line text box.  \
The usual control keys that work on other text editors will work  \
here too; see the later description of Menu Item: Edit.  \
<BR>  \
<BR>The program includes a tokenizer and can recognize the parts of  \
speech in typical MySQL grammar. It will do syntax highlighting  \
by changing the color, for example comments will appear in red,  \
dentifiers in blue, operators in green, and so on.  \
The colors can be  \
customized, see the later description of Menu Item: Settings.  \
<BR>  \
<BR>The left side of the statement widget is reserved for the prompt,  \
and cannot be typed over. Initially the prompt will be 'mysql&gt;',  \
but this can be changed, see the later description of  \
Client Statements: Prompt.  \
<BR>  \
Once a statement has been entered and is ready to be executed,  \
the user can hit control-E, choose menu item Run|Execute, or  \
place the cursor at the end of the text (after the ';' or other  \
delimiter) and type Enter. It is legal to enter multiple  \
statements, separated by semicolons, and then execute them  \
in a single sequence.  \
<BR>  \
<BR><h2>Client statements</h2>  \
<BR>  \
<BR>A client statement is a statement which changes some behavior  \
of the client (that is, of the ocelotgui front end) but does not  \
necessarily go to the MySQL/MariaDB server. Of the statements  \
that the MySQL Reference manual describes in section  \
'mysql commands' http://dev.mysql.com/doc/refman/5.6/en/mysql-commands.html.  \
<BR>  \
<BR>The ocelotgui program has working equivalents for: clear, delimiter, exit,  \
prompt, source, and warnings. For example, entering 'quit;'  \
followed by Enter will cause the program to stop. It is  \
sometimes not mandatory to end a client statement with ';',  \
but is strongly recommended.  \
<BR>  \
<BR>There are some enhancements affecting the PROMPT statement.  \
The special sequence '&#92;2' means 'repeat the prompt on all lines',  \
and the special sequence '&#92;L' means 'show line numbers'. For example,  \
'PROMPT &#92;2&#92;Lmariadb;' will change the prompt so that each line begins  \
with '[line number] mariadb>'.  \
<BR>  \
<BR><h2>History widget</h2>  \
<BR>  \
<BR>Once a statement has been executed, a copy of the statement text  \
and the diagnostic result (for example: 0.04 seconds, OK) will  \
be placed in the history widget. Everything in the history widget  \
is editable including the prompt, and it simply fills up so that  \
after a while the older statements are scrolled off the screen.  \
Thus its main function is to show what recent statements and  \
results were. Statements in the history can be retrieved while  \
the focus is on the statement widget, by selecting 'Previous statement'  \
or 'Next statement' menu items.  \
<BR>  \
<BR><h2>Result widget</h2>  \
<BR>  \
<BR>If a statement is SELECT or SHOW or some other statement that  \
returns a result set, it will appear in the result widget in  \
the middle area of the screen. The result widget is split up  \
into columns. Each column has a header and details taken from  \
what the DBMS returns.  \
<BR>  \
<BR>The width of the column depends on the result set's definition,  \
but extremely wide columns will be split onto multiple lines.  \
That is, one result-set row may take up to five lines.  \
If the data still is too wide or too tall to fit in the cell,  \
then the cell will get a vertical scroll bar. The user can  \
change the width of a column by dragging the column's right  \
border to the right to make the column wider, or to the left  \
to make it narrower.  \
<BR>  \
The result widget as a whole may have a horizontal and a vertical  \
scroll bar. The vertical scroll bar moves a row at a time rather  \
than a pixel at a time -- this makes large result sets more  \
manageable, but makes the vertical scroll bar unresponsive if  \
each row has multiple lines and the number of rows is small.  \
<BR>  \
<BR><h2>Menu</h2>  \
<BR>  \
<BR>The menu at the top of the screen has File, Settings, Run,  \
Exit and Help.  \
<BR>  \
<BR>File|Connect, or Ctrl+O, starts the Connect dialog box.  \
<BR>File|Exit, or Ctrl+Q, stops the program.  \
<BR>  \
<BR>Edit|Undo or Ctrl+Z, , Edit|Redo or Ctrl+Shift+Z, Edit|Cut  \
or Ctrl+X, Edit|Copy , Edit|Cut, Edit|Copy, Edit|Paste,  \
and Edit|Select All work in the conventional manner,  \
except Edit|Redo which can only redo the last change.  \
<BR>  \
<BR>Run|Execute or Ctrl+E or Ctrl+Enter causes execution of whatever is in the  \
statement widget.  \
<BR>  \
<BR>Settings|Statement Widget: Colors + Fonts,  \
Settings|Grid Widget: Colors + Fonts,  \
Settings|History Widget: Colors + Fonts, and  \
Settings|Main Widget: Colors + Fonts are the  \
items which affect the behavior of each of each  \
individual widget. The color settings affect foregrounds,  \
backgrounds, borders, and (for the statement widget only)  \
the syntax highlights. The font settings affect font family,  \
boldness, italics, and size. Both color and font settings  \
involve further dialog boxes which are standard with Qt.  \
There may be additional choices affecting appearance,  \
for example the width of the border used to drag columns  \
in the result widget.  \
<BR>  \
<BR>Help|About will show the license and copyright and version.  \
Help|Manual will show the manual.  \
<BR>  \
<BR><h2>Debugger</h2>  \
NEW: it's possible to debug stored procedures and functions.  \
This version of ocelotgui incorporates MDBug  \
(read about MDBug at http://bazaar.launchpad.net/~hp-mdbug-team/mdbug/trunk/view/head:/debugger.txt).  \
All debugger insructions can be entered on the ocelotgui command line;  \
some operations can also be done via the Debug menu or by clicking on the stored-procedure display.  \
Currently-supported instructions are:  \
<BR>$install -- this is always the first thing to do  \
<BR>$setup routine_name [, routine_name ...] -- prepares so '$debug routine_name' is possible  \
<BR>$debug routine_name -- starts a debug session, shows routines in a tabbed widget  \
<BR>$breakpoint routine_name line_number or Debug|breakpoint -- sets a breakpoint  \
<BR>$clear routine_name line_number -- clears a breakpoint  \
<BR>$next or Debug|Next -- goes to next executable line, without dropping into subroutines  \
<BR>$step or Debug|Step -- goes to next executable line, will drop into subroutines  \
<BR>$continue or Debug|Continue -- executes until breakpoint or until end of procedure  \
<BR>$refresh breakpoints -- refreshes xxxmdbug.breakpoints table  \
<BR>$refresh server_variables -- refreshes xxxmdbug.server_variables table  \
<BR>$refresh variables -- refreshes xxxmdbug.variables table  \
<BR>$refresh user_variables -- refereshes xxxmdbug.user_variables table  \
<BR>$exit or Debug|Exit -- stops a debug session  \
<BR>  \
For a walk through a debugger example, with screenshots, see  \
this blog post: ocelot.ca/blog/the-ocelotgui-debugger.  \
<BR>  \
<BR><h2>Contact</h2>  \
<BR>  \
<BR>Bug reports and feature requests may go on  \
https://github.com/ocelot-inc/ocelotgui/issues.  \
<BR>  \
There may be announcements from time to time on Ocelot's  \
web page (ocelot.ca) or on the employee blog (ocelot.ca/blog).  \
This manual will also be available on ocelot.ca soon.  \
<BR>  \
<BR>Any contributions will be appreciated.  \
  ");
  }

void MainWindow::action_the_manual()
{
  the_manual_widget->setMinimumSize(500,500);
  the_manual_widget->show();
  the_manual_widget->raise();
}


/* For the Close button of Menu | Help | the_manual */
void MainWindow::action_the_manual_close()
{
  the_manual_widget->hide();
}


/*
  Problem: to do syntax highlighting, one must have a slot
  action_statement_edit_widget_text_changed() to intercept
  every change in action_statement_edit_widget. But, when
  user chooses Undo, that interception happens again, which
  negates the effects of undo. Also, undo has to be done
  twice, because everythng between beginEditBlock() and
  endEditBlock() is an undoable change. Therefore, instead
  of getting slot() invoked directly, I get to action_undo()
  which temporarily disables use of action_statement_edit_widget_changed()
  and calls undo() twice.
  Todo: consider: what if the user wants to undo with control-Z instead of menu?
  Todo: consider: will there be a bug if syntax highlighting is disabled?
  Todo: consider: perhaps now redo will be no good.
*/
void MainWindow::action_undo()
{
  statement_edit_widget_text_changed_flag= 1;
  statement_edit_widget->undo();
  statement_edit_widget->undo();
  statement_edit_widget_text_changed_flag= 0;
}


/*
  Comments relevant to action_statement(), action_grid(), action_history(), action_main() ...
  These let the user select the colors and font for some widgets.
  Qt documentation says: "QApplication maintains a system/theme font which serves as a default for all widgets."
  So I'll assume that the Main Window always has that font, but let user change widgets that are within it.
  It's best to use a fixed-width font for most widgets, but I won't force that.
  Todo: consider: Should we pass default = current font (what we changed to last time) rather than default = main window font?
  Todo: Looks like there's a small problem with the history_edit_widget -- it might scroll.
        Probably what we want is: if we're at the bottom before the font change, then move to the bottom after the change.
*/
/* Todo: before se->delete(), check if WA_DeleteOnClose is off, always, for sure. */
/* Todo: edit_widget_text_changed() etc. should only be done if one of the ocelot_statement_highlight variables changed */
/* Todo: edit_widget_text_changed() etc. should be preceded by "emit"? */
/* Todo: put setStyleSheet() in an "if": only redo if ocelot_statement_style_string changed. */
/* Todo: after doing a setStyleSheet() change, put in history as "SET @..." statements. */
/* Todo: make sure there's nno need to call set_current_colors_and_font(); */
/*
   Todo: Get default color too.
   I can get colors too, based on
   color= widget.palette().color(QPalette::Window);            formerly known as QPalette::Background
   color= widget.palette().color(QPalette::WindowText);           formerly known as QPalette::Foreground
   but    color.name() gives the color as "#RRGGBB" -- but I want a name!
   But your problem is: it might be RGB originally. You'd have to accept RGB.
   Luckily setStyleSheet will accept it, e.g. statement_edit_widget->setStyleSheet("background:rgb(200,100,150);");
   See also http://www.w3.org/TR/SVG/types.html#ColorKeywords "recognized color keyword names".
*/

void MainWindow::action_statement()
{
  Settings *se= new Settings(STATEMENT_WIDGET, this);
  int result= se->exec();
  if (result == QDialog::Accepted)
  {
    //make_style_strings();
    //statement_edit_widget->setStyleSheet(ocelot_statement_style_string);
    /* For each changed Settings item, produce and execute a settings-change statement. */
    action_change_one_setting(ocelot_statement_color, new_ocelot_statement_color,"ocelot_statement_color");
    action_change_one_setting(ocelot_statement_background_color,new_ocelot_statement_background_color,"ocelot_statement_background_color");
    action_change_one_setting(ocelot_statement_border_color,new_ocelot_statement_border_color,"ocelot_statement_border_color");
    action_change_one_setting(ocelot_statement_font_family,new_ocelot_statement_font_family,"ocelot_statement_font_family");
    action_change_one_setting(ocelot_statement_font_size,new_ocelot_statement_font_size,"ocelot_statement_font_size");
    action_change_one_setting(ocelot_statement_font_style,new_ocelot_statement_font_style,"ocelot_statement_font_style");
    action_change_one_setting(ocelot_statement_font_weight,new_ocelot_statement_font_weight,"ocelot_statement_font_weight");
    action_change_one_setting(ocelot_statement_highlight_literal_color,new_ocelot_statement_highlight_literal_color,"ocelot_statement_highlight_literal_color");
    action_change_one_setting(ocelot_statement_highlight_identifier_color, new_ocelot_statement_highlight_identifier_color,"ocelot_statement_highlight_identifier_color");
    action_change_one_setting(ocelot_statement_highlight_comment_color, new_ocelot_statement_highlight_comment_color,"ocelot_statement_highlight_comment_color");
    action_change_one_setting(ocelot_statement_highlight_operator_color, new_ocelot_statement_highlight_operator_color,"ocelot_statement_highlight_operator_color");
    action_change_one_setting(ocelot_statement_highlight_reserved_color, new_ocelot_statement_highlight_reserved_color,"ocelot_statement_highlight_reserved_color");
    action_change_one_setting(ocelot_statement_prompt_background_color, new_ocelot_statement_prompt_background_color,"ocelot_statement_prompt_background_color");

    /* Todo: consider: maybe you hve to do a restore like this */
    //text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */
    //tokenize(text.data(),
    //         text.size(),
    //         &main_token_lengths, &main_token_offsets, MAX_TOKENS,(QChar*)"33333", 1, ocelot_delimiter_str, 1);
    //tokens_to_keywords(text);
    /* statement_edit_widget->statement_edit_widget_left_bgcolor= QColor(ocelot_statement_prompt_background_color); */
    statement_edit_widget->statement_edit_widget_left_treatment1_textcolor= QColor(ocelot_statement_color);
    action_statement_edit_widget_text_changed();            /* only for highlight? repaint so new highlighting will appear */
  }
  delete(se);
}

/*
  Todo: the setstylesheet here takes a long time because there are so many child widgets.
  One way to speed it up could be: invalidate the text_edit_frames the way you do for content
*/
void MainWindow::action_grid()
{
  Settings *se= new Settings(GRID_WIDGET, this);
  int result= se->exec();
  if (result == QDialog::Accepted)
  {
    //make_style_strings();                                                      /* I think this should be commented out */
    //result_grid_table_widget[0]->set_all_style_sheets();
    /* For each changed Settings item, produce and execute a settings-change statement. */
    action_change_one_setting(ocelot_grid_color, new_ocelot_grid_color, "ocelot_grid_color");
    action_change_one_setting(ocelot_grid_border_color, new_ocelot_grid_border_color, "ocelot_grid_border_color");
    action_change_one_setting(ocelot_grid_background_color, new_ocelot_grid_background_color, "ocelot_grid_background_color");
    action_change_one_setting(ocelot_grid_header_background_color, new_ocelot_grid_header_background_color, "ocelot_grid_header_background_color");
    action_change_one_setting(ocelot_grid_font_family, new_ocelot_grid_font_family, "ocelot_grid_font_family");
    action_change_one_setting(ocelot_grid_font_size, new_ocelot_grid_font_size, "ocelot_grid_font_size");
    action_change_one_setting(ocelot_grid_font_style, new_ocelot_grid_font_style, "ocelot_grid_font_style");
    action_change_one_setting(ocelot_grid_font_weight, new_ocelot_grid_font_weight, "ocelot_grid_font_weight");
    action_change_one_setting(ocelot_grid_cell_border_color, new_ocelot_grid_cell_border_color, "ocelot_grid_cell_border_color");
    action_change_one_setting(ocelot_grid_cell_right_drag_line_color, new_ocelot_grid_cell_right_drag_line_color, "ocelot_grid_cell_right_drag_line_color");
    action_change_one_setting(ocelot_grid_border_size, new_ocelot_grid_border_size, "ocelot_grid_border_size");
    action_change_one_setting(ocelot_grid_cell_border_size, new_ocelot_grid_cell_border_size, "ocelot_grid_cell_border_size");
    action_change_one_setting(ocelot_grid_cell_right_drag_line_size, new_ocelot_grid_cell_right_drag_line_size, "ocelot_grid_cell_right_drag_line_size");
  }
  delete(se);
}

void MainWindow::action_history()
{
  Settings *se= new Settings(HISTORY_WIDGET, this);
  int result= se->exec();
  if (result == QDialog::Accepted)
  {
    //make_style_strings();
    //history_edit_widget->setStyleSheet(ocelot_history_style_string);
    action_change_one_setting(ocelot_history_color, new_ocelot_history_color, "ocelot_history_color");
    action_change_one_setting(ocelot_history_background_color, new_ocelot_history_background_color, "ocelot_history_background_color");
    action_change_one_setting(ocelot_history_border_color, new_ocelot_history_border_color, "ocelot_history_border_color");
    action_change_one_setting(ocelot_history_font_family, new_ocelot_history_font_family, "ocelot_history_font_family");
    action_change_one_setting(ocelot_history_font_size, new_ocelot_history_font_size, "ocelot_history_font_size");
    action_change_one_setting(ocelot_history_font_style, new_ocelot_history_font_style, "ocelot_history_font_style");
    action_change_one_setting(ocelot_history_font_weight, new_ocelot_history_font_weight, "ocelot_history_font_weight");
  }
  delete(se);
}

/* Todo: consider: maybe changing should only affect menuBar so there's no inheriting. */
void MainWindow::action_main()
{
  Settings *se= new Settings(MAIN_WIDGET, this);
  int result= se->exec();
  if (result == QDialog::Accepted)
  {
    //make_style_strings();
    //main_window->setStyleSheet(ocelot_main_style_string);
    //ui->menuBar->setStyleSheet(ocelot_main_style_string);
      action_change_one_setting(ocelot_main_color, new_ocelot_main_color, "ocelot_main_color");
      action_change_one_setting(ocelot_main_background_color, new_ocelot_main_background_color, "ocelot_main_background_color");
      action_change_one_setting(ocelot_main_border_color, new_ocelot_main_border_color, "ocelot_main_border_color");
      action_change_one_setting(ocelot_main_font_family, new_ocelot_main_font_family, "ocelot_main_font_family");
      action_change_one_setting(ocelot_main_font_size, new_ocelot_main_font_size, "ocelot_main_font_size");
      action_change_one_setting(ocelot_main_font_style, new_ocelot_main_font_style, "ocelot_main_font_style");
      action_change_one_setting(ocelot_main_font_weight, new_ocelot_main_font_weight, "ocelot_main_font_weight");
  }
  delete(se);
}


/*
  If a Settings-menu use has caused a change:
  Produce a settings-change SQL statement, e.g.
  SET @ocelot_statement_background_color = 'red'
  and execute it.
*/
void MainWindow::action_change_one_setting(QString old_setting, QString new_setting, const char *name_of_setting)
{
  if (old_setting != new_setting)
  {
    QString text;
    old_setting= new_setting;
    main_token_number= 0;
    text= "SET @";
    text.append(name_of_setting);
    text.append(" = '");
    text.append(new_setting);
    text.append("'");
    main_token_count_in_statement= 4;
    tokenize(text.data(),
             text.size(),
             &main_token_lengths, &main_token_offsets, MAX_TOKENS,(QChar*)"33333", 1, ocelot_delimiter_str, 1);
    tokens_to_keywords(text);
    action_execute_one_statement(text);
  }
}


/*
  Todo: something has to be figured out about border width and border colors.
  I think, uncertainly, that the default border width is 0 and the color is Palette:shadow i.e. black.
  Possibly I could get the original value from QWidget::styleSheet() is some startup parameter changed it?
  Possibly I'd need to get a QFrame with a lineWidth() and frameWidth()?
  Possibly I should just assume that border width = 0px except for main window, and
  take out any mention of border in the Settings dialogs.
*/
void MainWindow::set_current_colors_and_font()
{
  QFont font;
  ocelot_statement_color= statement_edit_widget->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_statement_background_color= statement_edit_widget->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  font= statement_edit_widget->font();
  ocelot_statement_font_family= font.family();
  if (font.italic()) ocelot_statement_font_style= "italic"; else ocelot_statement_font_style= "normal";
  ocelot_statement_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
  if (font.weight() >= QFont::Bold) ocelot_statement_font_weight= "bold"; else ocelot_statement_font_weight= "normal";

  ocelot_grid_color= result_grid_table_widget[0]->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_grid_background_color=result_grid_table_widget[0]->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  font= result_grid_table_widget[0]->font();
  ocelot_grid_font_family= font.family();
  if (font.italic()) ocelot_grid_font_style= "italic"; else ocelot_grid_font_style= "normal";
  ocelot_grid_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
  if (font.weight() >= QFont::Bold) ocelot_grid_font_weight= "bold"; else ocelot_grid_font_weight= "normal";

  ocelot_history_color= history_edit_widget->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_history_background_color=history_edit_widget->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  font= history_edit_widget->font();
  ocelot_history_font_family= font.family();
  if (font.italic()) ocelot_history_font_style= "italic"; else ocelot_history_font_style= "normal";
  ocelot_history_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
  if (font.weight() >= QFont::Bold) ocelot_history_font_weight= "bold"; else ocelot_history_font_weight= "normal";

  ocelot_main_color= main_window->palette().color(QPalette::WindowText).name(); /* = QPalette::Foreground */
  ocelot_main_background_color= main_window->palette().color(QPalette::Window).name(); /* = QPalette::Background */
  font= main_window->font();
  ocelot_main_font_family= font.family();
  if (font.italic()) ocelot_main_font_style= "italic"; else ocelot_main_font_style= "normal";
  ocelot_main_font_size= QString::number(font.pointSize()); /* Warning: this returns -1 if size was specified in pixels */
  if (font.weight() >= QFont::Bold) ocelot_main_font_weight= "bold"; else ocelot_main_font_weight= "normal";
}


/* Called from: action_statement() etc. Make a string that setStyleSheet() can use. */
/*
  Todo: I wasn't able to figure out a simple way to emphasize widget title, for example make it bold.
  Todo: Set a border around a widget if it's detached, when detaching is supported again.
*/
void MainWindow::make_style_strings()
{
  ocelot_statement_style_string= "color:"; ocelot_statement_style_string.append(ocelot_statement_color);
  ocelot_statement_style_string.append(";background-color:"); ocelot_statement_style_string.append(ocelot_statement_background_color);
  ocelot_statement_style_string.append(";border:1px solid "); ocelot_statement_style_string.append(ocelot_statement_border_color);
  ocelot_statement_style_string.append(";font-family:"); ocelot_statement_style_string.append(ocelot_statement_font_family);
  ocelot_statement_style_string.append(";font-size:"); ocelot_statement_style_string.append(ocelot_statement_font_size);
  ocelot_statement_style_string.append("pt;font-style:"); ocelot_statement_style_string.append(ocelot_statement_font_style);
  ocelot_statement_style_string.append(";font-weight:"); ocelot_statement_style_string.append(ocelot_statement_font_weight);

  ocelot_grid_style_string= "color:"; ocelot_grid_style_string.append(ocelot_grid_color);
  ocelot_grid_style_string.append(";background-color:"); ocelot_grid_style_string.append(ocelot_grid_background_color);
  ocelot_grid_style_string.append(";border:");
  ocelot_grid_style_string.append(ocelot_grid_cell_border_size);
  ocelot_grid_style_string.append("px solid ");
  ocelot_grid_style_string.append(ocelot_grid_border_color);
  ocelot_grid_style_string.append(";font-family:"); ocelot_grid_style_string.append(ocelot_grid_font_family);
  ocelot_grid_style_string.append(";font-size:"); ocelot_grid_style_string.append(ocelot_grid_font_size);
  ocelot_grid_style_string.append("pt;font-style:"); ocelot_grid_style_string.append(ocelot_grid_font_style);
  ocelot_grid_style_string.append(";font-weight:"); ocelot_grid_style_string.append(ocelot_grid_font_weight);

  ocelot_grid_header_style_string= "color:"; ocelot_grid_header_style_string.append(ocelot_grid_color);
  ocelot_grid_header_style_string.append(";background-color:"); ocelot_grid_header_style_string.append(ocelot_grid_header_background_color);
  ocelot_grid_header_style_string.append(";border:");
  ocelot_grid_header_style_string.append(ocelot_grid_cell_border_size);
  ocelot_grid_header_style_string.append("px solid ");
  ocelot_grid_header_style_string.append(ocelot_grid_border_color);
  ocelot_grid_header_style_string.append(";font-family:"); ocelot_grid_header_style_string.append(ocelot_grid_font_family);
  ocelot_grid_header_style_string.append(";font-size:"); ocelot_grid_header_style_string.append(ocelot_grid_font_size);
  ocelot_grid_header_style_string.append("pt;font-style:"); ocelot_grid_header_style_string.append(ocelot_grid_font_style);
  ocelot_grid_header_style_string.append(";font-weight:"); ocelot_grid_header_style_string.append(ocelot_grid_font_weight);

  ocelot_history_style_string= "color:"; ocelot_history_style_string.append(ocelot_history_color);
  ocelot_history_style_string.append(";background-color:"); ocelot_history_style_string.append(ocelot_history_background_color);
  ocelot_history_style_string.append(";border:1px solid "); ocelot_history_style_string.append(ocelot_history_border_color);
  ocelot_history_style_string.append(";font-family:"); ocelot_history_style_string.append(ocelot_history_font_family);
  ocelot_history_style_string.append(";font-size:"); ocelot_history_style_string.append(ocelot_history_font_size);
  ocelot_history_style_string.append("pt;font-style:"); ocelot_history_style_string.append(ocelot_history_font_style);
  ocelot_history_style_string.append(";font-weight:"); ocelot_history_style_string.append(ocelot_history_font_weight);

  ocelot_main_style_string= "color:"; ocelot_main_style_string.append(ocelot_main_color);
  ocelot_main_style_string.append(";background-color:"); ocelot_main_style_string.append(ocelot_main_background_color);
  ocelot_main_style_string.append(";border:1px solid "); ocelot_main_style_string.append(ocelot_main_border_color);
  ocelot_main_style_string.append(";font-family:"); ocelot_main_style_string.append(ocelot_main_font_family);
  ocelot_main_style_string.append(";font-size:"); ocelot_main_style_string.append(ocelot_main_font_size);
  ocelot_main_style_string.append("pt;font-style:"); ocelot_main_style_string.append(ocelot_main_font_style);
  ocelot_main_style_string.append(";font-weight:"); ocelot_main_style_string.append(ocelot_main_font_weight);
}


/*
  Get the next statement in a string.
  Pass: string, offset within string. Or: main_token number.
  Return: length i.e. # of tokens including delimiter or ; so offset can be right next time
          length == 0 would mean end-of-string
  Re how to find the end of a statement:
     For each token
       If (token == BEGIN or ELSEIF or IF or LOOP or REPEAT or WHILE) ++begin_count
       If (token == END --begin_count
     If (token == delimiter) this is the end
     If (no more tokens) this is the end
     If (token == ';' and begin_count == 0) this is the end
     Todo: What about \G etc.?
     NB: it's an error if begin_count>0 and this is the end, caller might return client error
  Re CLIENT_MULTI_STATEMENT: this would work too, but in the long term I'll want to
     know for sure: which statement returned result set X? Since a CALL can return
     multiple result sets, I didn't see a way to figure that out.
  Don't bother with begin_count if PROCEDURE or FUNCTION or TRIGGER hasn't been seen.
  Beware: insert into t8 values (5); prompt w (unlike mysql client we wait for ';' here)
  Beware: create procedure p () begin end// select 5//
  Beware: input might be a file dump, and statements might be long.
  Todo: think about delimiter. Maybe delimiters don't count if you're in a delimiter statement?
  Todo: this doesn't allow for the possibility of "END comment WHILE|LOOP|REPEAT;"
*/
int MainWindow::get_next_statement_in_string(int passed_main_token_number, int *returned_begin_count)
{
  int i;
  int begin_count;
  QString last_token;
  QString text;

  text= statement_edit_widget->toPlainText(); /* Todo: decide whether I'm doing this too often */
  bool is_maybe_in_compound_statement= 0;
  begin_count= 0;
  for (i= passed_main_token_number; main_token_lengths[i] != 0; ++i)
  {
    last_token= text.mid(main_token_offsets[i], main_token_lengths[i]);
    if (QString::compare(ocelot_delimiter_str, ";", Qt::CaseInsensitive) != 0)
    {
      if (QString::compare(last_token, ocelot_delimiter_str, Qt::CaseInsensitive) == 0)
      {
        ++i; break;
      }
    }
    if ((QString::compare(last_token, ";", Qt::CaseInsensitive) == 0) && (begin_count == 0))
    {
      ++i; break;
    }
    if ((main_token_types[i] == TOKEN_KEYWORD_PROCEDURE)
    ||  (main_token_types[i] == TOKEN_KEYWORD_FUNCTION)
    ||  (main_token_types[i] == TOKEN_KEYWORD_TRIGGER))
    {
      is_maybe_in_compound_statement= 1;
    }
    if (is_maybe_in_compound_statement == 1)
    {
      if ((main_token_types[i] == TOKEN_KEYWORD_BEGIN)
      ||  (main_token_types[i] == TOKEN_KEYWORD_ELSEIF)
      ||  ((main_token_types[i] == TOKEN_KEYWORD_IF) && (main_token_types[i - 1] != TOKEN_KEYWORD_END))
      ||  ((main_token_types[i] == TOKEN_KEYWORD_LOOP) && (main_token_types[i - 1] != TOKEN_KEYWORD_END))
      ||  ((main_token_types[i] == TOKEN_KEYWORD_REPEAT) && (main_token_types[i - 1] != TOKEN_KEYWORD_END))
      ||  ((main_token_types[i] == TOKEN_KEYWORD_WHILE) && (main_token_types[i - 1] != TOKEN_KEYWORD_END)))
      {
        ++begin_count;
      }
      if (main_token_types[i] == TOKEN_KEYWORD_END)
      {
        --begin_count;
      }
    }
  }
  *returned_begin_count= begin_count;
  return i - passed_main_token_number;
}


/*
  Todo: This function should do something!
  Make statement ready to send to server: strip comments and delimiter, output UTF8.
  We have an SQL statement that's ready to go to the server.
  We have a guarantee that the result i.e. "char *query" is big enough.
  Rremove comments if that's what the user requested with --skip-comments.
  Remove final token if it's delimiter but not ';'.
  Todo: there's a conversion to UTF8 but it should be to what server expects.
  Todo: um, in that case, make server expect UTF8.
  Re comment stripping: currently this is default because in mysql client it's default
    but there should be no assumption that this will always be the case,
    so some sort of warning might be good. The history display has to include
    the comments even if they're not sent to the server, so the caller of this
    routine will use both (original string,offset,length) and (returned string).
    Comments should be replaced with a single space.
*/
int MainWindow::make_statement_ready_to_send(QString text, QString query_utf16_copy,
                                             char *dbms_query, int dbms_query_len,
                                             bool strip_last_token)
{
  if (ocelot_comments > 0)
  {
    memcpy(dbms_query, query_utf16_copy.toUtf8().constData(),dbms_query_len);
    dbms_query[dbms_query_len]= 0;
    return dbms_query_len;
  }
  int token_type;
  unsigned int i;
  QString q;
  char *tmp;
  int tmp_len;
  unsigned int token_count;
  dbms_query[0]= '\0';
  /* Ignore last token(s) if delimiter, \G, \g, or (sometimes) go, ego */
  if (strip_last_token == true)
  {
    int last_i= main_token_number + main_token_count_in_statement - 1;
    QString last_token= text.mid(main_token_offsets[last_i], main_token_lengths[last_i]);
    if ((last_token == "G") || (last_token == "g"))
    {
      token_count= main_token_count_in_statement - 2;
    }
    else token_count= main_token_count_in_statement - 1;
  }
  else token_count= main_token_count_in_statement;
  for (i= main_token_number; i < main_token_number + token_count; ++i)
  {
    token_type= main_token_types[i];
    if ((token_type == TOKEN_TYPE_COMMENT_WITH_SLASH)
     || (token_type == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     || (token_type == TOKEN_TYPE_COMMENT_WITH_MINUS))
     {
       strcat(dbms_query," ");
       continue;
     }
    q= text.mid(main_token_offsets[i], main_token_offsets[i + 1] - main_token_offsets[i]);
    tmp_len= q.toUtf8().size();           /* See comment "UTF8 Conversion" */
    tmp= new char[tmp_len + 1];
    memcpy(tmp, q.toUtf8().constData(), tmp_len);
    tmp[tmp_len]= 0;
    strcat(dbms_query, tmp);
    delete [] tmp;
  }
  return (strlen(dbms_query));
}

/*
  select_1_row() is a convenience routine, made because frequently there are single-row selects.
  And if they are multiple-row selects, we can always do them one-at-a-time with limit and offset clauses.
  Pass: select statement. Return: unexpected_error, expected to be ""
  And up to 5 string variables from the first row that gets selected.
  This should be used only if you know that no fields are null.
*/
QString select_1_row_result_1, select_1_row_result_2, select_1_row_result_3, select_1_row_result_4, select_1_row_result_5;
QString MainWindow::select_1_row(const char *select_statement)
{
  MYSQL_RES *res= NULL;
  MYSQL_ROW row= NULL;
  const char *unexpected_error= NULL;
  QString s;
  unsigned int num_fields;

  if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], select_statement, strlen(select_statement)))
  {
    unexpected_error= "select failed";
  }

  if (unexpected_error == NULL)
  {
    res= mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
    if (res == NULL)
    {
      unexpected_error= "mysql_store_result failed";
    }
  }

  if (unexpected_error == NULL)
  {
    num_fields= mysql_num_fields(res);
  }

  if (unexpected_error == NULL)
  {
    row= mysql_fetch_row(res);
    if (row == NULL)
    {
      unexpected_error= "mysql_fetch row failed"; /* Beware! Look for a proc that compares routine with this string value! */
    }
    else
    {
      if (num_fields > 0) select_1_row_result_1= QString::fromUtf8(row[0]);
      if (num_fields > 1) select_1_row_result_2= QString::fromUtf8(row[1]);
      if (num_fields > 2) select_1_row_result_3= QString::fromUtf8(row[2]);
      if (num_fields > 3) select_1_row_result_4= QString::fromUtf8(row[3]);
      if (num_fields > 4) select_1_row_result_5= QString::fromUtf8(row[4]);
    }
  }

  if (res != NULL) mysql_free_result(res);

  if (unexpected_error != NULL) s= unexpected_error;
  else s= "";

  return s;
}

/* Variables used by kill thread, but which might be checked by debugger */
#define KILL_STATE_CONNECT_THREAD_STARTED 0
#define KILL_STATE_CONNECT_FAILED 1
#define KILL_STATE_IS_CONNECTED 2
#define KILL_STATE_MYSQL_REAL_QUERY_ERROR 3
#define KILL_STATE_ENDED 4
int volatile kill_state;
int volatile kill_connection_id;

#define LONG_QUERY_STATE_STARTED 0
#define LONG_QUERY_STATE_ENDED 1
char *dbms_query;
int dbms_query_len;
volatile int dbms_long_query_result;
volatile int dbms_long_query_state= LONG_QUERY_STATE_ENDED;


#ifdef DEBUGGER

/*
  Debug

  Re debug_top_widget: It's a QWidget which contains a status line and debug_tab_widget.

  Re debug_tab_widget: It's a QTabWidget which may contain more than one debug_widget.

  Re debug_widget: it's derived from CodeEditor just as statement_edit_widget is, and it uses the
  same stylesheet as statement_edit_widget. It starts hidden, but debug|initialize shows it.
  It will contain the text of a procedure which is being debugged.
  Actually debug_widget[] occurs 10 times -- todo: there's no check for overflow.

  Re statements that come via statement_edit_widget: suppose the statement is "$DEB p8;".
  During tokens_to_keywords() we recognize that this is TOKEN_KEYWORD_DEBUG_DEBUG and highlight appropriately.
  During execute_client_statement(), after we check if statement_type == TOKEN_DEBUG_DEBUG_DEBUG and find that
  it's true, there's a call to action_debug_debug() followed by "return 1". When we're in action_debug_debug(),
  we have access to main_tokens so we can say things like "QString s= text.mid(main_token_offsets[i], main_token_lengths[i]);".
*/

/* This means "while debugger is going check status 20 times per second" (50 = 50 milliseconds). */
#define DEBUG_TIMER_INTERVAL 50

#define DEBUGGEE_STATE_0 0
#define DEBUGGEE_STATE_CONNECT_THREAD_STARTED 1
#define DEBUGGEE_STATE_BECOME_DEBUGEE_CONNECTION 2
#define DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP 3
#define DEBUGGEE_STATE_CONNECT_FAILED -1
#define DEBUGGEE_STATE_MYSQL_REAL_QUERY_ERROR -2
#define DEBUGGEE_STATE_MYSQL_STORE_RESULT_ERROR -3
#define DEBUGGEE_STATE_MYSQL_FETCH_ROW_ERROR -4
#define DEBUGGEE_STATE_BECOME_DEBUGGEE_CONNECTION_ERROR -5
#define DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR -6
#define DEBUGGEE_STATE_END -7

char debuggee_channel_name[128]= "";
int volatile debuggee_state= DEBUGGEE_STATE_0;
char debuggee_state_error[STRING_LENGTH_512]= "";
char debuggee_information_status_debugger_name[32];
char debuggee_information_status_debugger_version[8];
char debuggee_information_status_timestamp_of_status_message[32];
char debuggee_information_status_number_of_status_message[8];
char debuggee_information_status_icc_count[8];
char debuggee_information_status_schema_identifier[256];
char debuggee_information_status_routine_identifier[256];
char debuggee_information_status_line_number[8];
char debuggee_information_status_is_at_breakpoint[8];
char debuggee_information_status_is_at_tbreakpoint[8];
char debuggee_information_status_is_at_routine_exit[8];
char debuggee_information_status_stack_depth[8];
char debuggee_information_status_last_command[256];
char debuggee_information_status_last_command_result[256];
char debuggee_information_status_commands_count[8];

QString debug_routine_schema_name[DEBUG_TAB_WIDGET_MAX + 1];
QString debug_routine_name[DEBUG_TAB_WIDGET_MAX + 1];

QString debug_q_schema_name_in_statement;
QString debug_q_routine_name_in_statement;
QString debug_statement;

QTimer *debug_timer;                                    /* For calling action_debug_timer_status() every 1/10 seconds */
int debug_timer_old_line_number;
char debug_timer_old_schema_identifier[256];
char debug_timer_old_routine_identifier[256];
int volatile debuggee_connection_id;                             /* Save this if you need to kill the debuggee (an unusual need) */
int debug_timer_old_commands_count;
int debug_timer_old_number_of_status_message;
int debug_timer_old_icc_count;
int debug_timer_old_debug_widget_index;



/*
  Enable or disable debug menu items.
  If debug succeeds: most items after debug are enabled. (Todo: eventually all should be enabled.)
  If exit succeeds: most items after debug are disabled.
*/
void MainWindow::debug_menu_enable_or_disable(int statement_type)
{
  bool e;

  if (statement_type == TOKEN_KEYWORD_BEGIN) e= false;
  if (statement_type == TOKEN_KEYWORD_DEBUG_DEBUG) e= true;
  if (statement_type == TOKEN_KEYWORD_DEBUG_EXIT) e= false;

//  menu_debug_action_install->setEnabled(e);
//  menu_debug_action_setup->setEnabled(e);
//  menu_debug_action_debug->setEnabled(e);
  menu_debug_action_breakpoint->setEnabled(e);
  menu_debug_action_continue->setEnabled(e);
//  menu_debug_action_leave->setEnabled(e);
  menu_debug_action_next->setEnabled(e);
//  menu_debug_action_skip->setEnabled(e);
  menu_debug_action_step->setEnabled(e);
  menu_debug_action_clear->setEnabled(e);
//  menu_debug_action_delete->setEnabled(e);
  menu_debug_action_exit->setEnabled(e);                                      /* everything after debug goes gray after this succeeds */
  menu_debug_action_information->setEnabled(e);
  menu_debug_action_refresh_server_variables->setEnabled(e);
  menu_debug_action_refresh_user_variables->setEnabled(e);
  menu_debug_action_refresh_variables->setEnabled(e);
  menu_debug_action_refresh_call_stack->setEnabled(e);
}


void MainWindow::create_widget_debug()
{
  /* We make debug_tab_widget now but we don't expect anyone to see it until debug|setup happens. */
  /* Todo: should you create with an initially-hidden flag? */
  debug_top_widget= new QWidget();
  debug_top_widget->hide();                         /* hidden until debug|initialize is executed */

  /*
    Todo: check: am I doing it right by saying parent = debug_top_widget
    i.e. will automatic delete happen when debug_top_widget is deleted?
    or will there be a memory leak?
    Hmm, why do I care? I don't delete debug_top_widget anyway, do I?
    Hmm Hmm, yes I do but it must be an error.
  */

  debug_line_widget= new QLineEdit(debug_top_widget);
  debug_line_widget->setText("Debugger Status = Inactive");
  debug_tab_widget= new QTabWidget(debug_top_widget);

  debug_top_widget_layout= new QVBoxLayout(debug_top_widget);
  debug_top_widget_layout->addWidget(debug_line_widget);
  debug_top_widget_layout->addWidget(debug_tab_widget);
  debug_top_widget->setLayout(debug_top_widget_layout);

  /* We make debug_timer now but we don't start it until debug status can be shown. */
  debug_timer= new QTimer(this);
  connect(debug_timer, SIGNAL(timeout()), this, SLOT(action_debug_timer_status()));
}


#include <pthread.h>

/*
  The debuggee is a separate thread and makes its own connection using ocelot_ parameters just like the main connection.
  Problem: debuggee_thread() cannot be in MainWindow (at least, I didn't figure out how to make it in MainWindow).
  That explains why I made a lot of things global.
  Todo: Check: Why do you end statements with ";"?
  Todo: If an error occurs, copy the whole thing into a fixed-size char so debugger can see it.
        And notice that the result is that the thread ends, so beware of racing and worry about cleanup.
*/

void* debuggee_thread(void* unused)
{
  (void) unused; /* suppress "unused parameter" warning */
  char call_statement[512];
  int is_connected= 0;
  MYSQL_RES *debug_res= NULL;
  MYSQL_ROW debug_row= NULL;
  const char *unexpected_error= NULL;

  debuggee_connection_id= 0;

  memset(debuggee_state_error, 0, STRING_LENGTH_512);
  debuggee_state= DEBUGGEE_STATE_CONNECT_THREAD_STARTED;

  for (;;)
  {
    /*
      The debuggee connection.
      I don't think I need to say mysql_init(&mysql[connection_number]);
      Todo: the_connect() could easily fail: parameters are changed, # of connections = max, password no longer works, etc.
            ... so you should try to pass back a better explanation if the_connect() fails here.
    */
    if (options_and_connect(MYSQL_DEBUGGER_CONNECTION))
    {
       debuggee_state= DEBUGGEE_STATE_CONNECT_FAILED;
       strncpy(debuggee_state_error, mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
       break;
    }

    is_connected= 1;

    /* Get connection_id(). This is only in case regular "exit" fails and we have to issue a kill statement. */
    strcpy(call_statement, "select connection_id();");
    if (mysql_real_query(&mysql[MYSQL_DEBUGGER_CONNECTION], call_statement, strlen(call_statement)))
    {
      debuggee_state= DEBUGGEE_STATE_MYSQL_REAL_QUERY_ERROR;
      strncpy(debuggee_state_error, mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
      break;
    }
    debug_res= mysql_store_result(&mysql[MYSQL_DEBUGGER_CONNECTION]);
    if (debug_res == NULL)
    {
      debuggee_state= DEBUGGEE_STATE_MYSQL_STORE_RESULT_ERROR;
      strncpy(debuggee_state_error, mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
      break;
    }
    debug_row= mysql_fetch_row(debug_res);
    if (debug_row == NULL)
    {
      debuggee_state= DEBUGGEE_STATE_MYSQL_FETCH_ROW_ERROR;
      strncpy(debuggee_state_error, mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
      mysql_free_result(debug_res);
      break;
    }
    debuggee_connection_id= atoi(debug_row[0]);
    mysql_free_result(debug_res);

    strcpy(call_statement, "call xxxmdbug.become_debuggee_connection('");
    strcat(call_statement, debuggee_channel_name);
    strcat(call_statement, "');");
    debuggee_state= DEBUGGEE_STATE_BECOME_DEBUGEE_CONNECTION;
    if (mysql_real_query(&mysql[MYSQL_DEBUGGER_CONNECTION], call_statement, strlen(call_statement)))
    {
      debuggee_state= DEBUGGEE_STATE_BECOME_DEBUGGEE_CONNECTION_ERROR;
      strncpy(debuggee_state_error, mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
      break;
    }

    /*
      Originally we just had debuggee_wait_loop here.
      Then the PREPARE+EXECUTE failed because of a procedure that contained a SELECT.
      So I replaced debuggee_wait_loop()'s PREPARE with "LEAVE".
      The actual execution will be done here in C.
      Todo: Replace all debuggee_wait_loop() with a C routine.
      Todo: Catch errors -- debuggee_wait_loop() didn't necessarily end via LEAVE x.
    */

    strcpy(call_statement, "call xxxmdbug.debuggee_wait_loop();");
    debuggee_state= DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP;
    if (mysql_real_query(&mysql[MYSQL_DEBUGGER_CONNECTION], call_statement, strlen(call_statement)))
    {
      debuggee_state= DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR;
      strncpy(debuggee_state_error, mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
      break;
    }
    {
      char xxxmdbug_status_last_command_result[512];
      if (mysql_query(&mysql[MYSQL_DEBUGGER_CONNECTION], "select @xxxmdbug_status_last_command_result"))
      {
        /* This can happen, for example if debug command failed. */
        debuggee_state= DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR;
        unexpected_error=" select @xxxmdbug_status_last_command_result failed";
        strcpy(debuggee_state_error, "no result from last command");
        break;
      }
      debug_res= mysql_store_result(&mysql[MYSQL_DEBUGGER_CONNECTION]);
      if (debug_res == NULL)
      {
        unexpected_error= "mysql_store_result failed";
      }
      if (mysql_num_fields(debug_res) != 1)
      {
        unexpected_error= "wrong field count";
        mysql_free_result(debug_res);
        break;
      }
      debug_row= mysql_fetch_row(debug_res);
      if (debug_row == NULL)
      {
        unexpected_error= "mysql_fetch_row failed";
        mysql_free_result(debug_res);
       break;
      }
      strcpy(xxxmdbug_status_last_command_result, debug_row[0]);
      mysql_free_result(debug_res);
      /* There are various errors that xxxmdbug.check_surrogate_routine could have returned. */
      if ((strstr(xxxmdbug_status_last_command_result,"Error reading setup log") != 0)
       || (strstr(xxxmdbug_status_last_command_result,"is incorrect format for a surrogate identifier") != 0)
       || (strstr(xxxmdbug_status_last_command_result,"is not found once in xxxmdbug.setup_log") != 0)
       || (strstr(xxxmdbug_status_last_command_result,"was set up with an older debugger version") != 0)
       || (strstr(xxxmdbug_status_last_command_result,"original was not found") != 0)
       || (strstr(xxxmdbug_status_last_command_result,"original was altered") != 0)
       || (strstr(xxxmdbug_status_last_command_result,"surrogate was not found") != 0))
      {
        debuggee_state= DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR;
        strncpy(debuggee_state_error, xxxmdbug_status_last_command_result, STRING_LENGTH_512 - 1);
        break;
      }
    }
    if (mysql_query(&mysql[MYSQL_DEBUGGER_CONNECTION], "select @xxxmdbug_what_to_call"))
    {
      unexpected_error=" select @xxxmdbug_what_to_call failed";
      break;
    }
    debug_res= mysql_store_result(&mysql[MYSQL_DEBUGGER_CONNECTION]);
    if (debug_res == NULL)
    {
      unexpected_error= "mysql_store_result failed";
    }
    if (mysql_num_fields(debug_res) != 1)
    {
      unexpected_error= "wrong field count";
      mysql_free_result(debug_res);
      break;
    }
    debug_row= mysql_fetch_row(debug_res);
    if (debug_row == NULL)
    {
      unexpected_error= "mysql_fetch_row failed";
      mysql_free_result(debug_res);
      break;
    }
    strcpy(call_statement, debug_row[0]);
    mysql_free_result(debug_res);

    if (mysql_real_query(&mysql[MYSQL_DEBUGGER_CONNECTION], call_statement, strlen(call_statement)))
    {
      debuggee_state= DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR;
      strncpy(debuggee_state_error, mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
    }
    else
    {
        /*
          Following is pretty much the same as for MYSQL_MAIN_CONNECTION.
          We're just throwing result sets away. NB: the statement has not
          really ended -- when there's a result set MySQL temporarily drops
          out of the procedure, and resumes during each iteration of this loop.
          Todo: we can put each result into the result grid widget as it comes;
          we'll have to decide whether the debugger should do it, or the main.
        */
        if (mysql_more_results(&mysql[MYSQL_DEBUGGER_CONNECTION]))
        {
          debug_res= mysql_store_result(&mysql[MYSQL_DEBUGGER_CONNECTION]);
          if (debug_res != 0)
          {
            mysql_free_result(debug_res);

            if (mysql_more_results(&mysql[MYSQL_DEBUGGER_CONNECTION]))
            {
              while (mysql_next_result(&mysql[MYSQL_DEBUGGER_CONNECTION]) == 0)
              {
                debug_res= mysql_store_result(&mysql[MYSQL_DEBUGGER_CONNECTION]);
                if (debug_res != 0) mysql_free_result(debug_res);
              }
              debug_res= 0;
            }
          }
        }

      debuggee_state= DEBUGGEE_STATE_END;
    }
    break;
  }

  /* Todo: Even after failure, get rid of result sets. But, oddly, mysql_next_result() never returns anything. */
  /* This just would throw any results away, which is what happens also with MYSQL_MAIN_CONNECTION. */
  /* What I'd have liked to do is: have the main connection put the result set up on the main screen. */
  //while (mysql_next_result(&mysql[MYSQL_DEBUGGER_CONNECTION]) == 0) ;
  //if (mysql_more_results(&mysql[MYSQL_DEBUGGER_CONNECTION]))
  //{
  //  while (mysql_next_result(&mysql[MYSQL_DEBUGGER_CONNECTION]) == 0)
  //  {
  //    debug_res= mysql_store_result(&mysql[MYSQL_DEBUGGER_CONNECTION]);
  //    if (debug_res != 0) mysql_free_result(debug_res);
  //  }
  //  debug_res= 0;
  //}
  if (unexpected_error != NULL)
  {
    strncpy(debuggee_state_error, mysql_error(&mysql[MYSQL_DEBUGGER_CONNECTION]), STRING_LENGTH_512 - 1);
  }

  /* Cleanup */
  /* Shut the connection, which seems to cause disconnect automatically. */
  if (is_connected == 1) mysql_close(&mysql[MYSQL_DEBUGGER_CONNECTION]);

  /* Typically we'll reach this point with last_command_result = "debuggee_wait_loop() termination" */

  /* Here I am overwriting DEBUGGEE_WAIT_LOOP_ERROR / DEBUGGEE_STATE_END. Maybe they're informative but they caused trouble. */
  //debuggee_state= DEBUGGEE_STATE_0;

  /* The thread will end, but mysql[MYSQL_DEBUGGER_CONNECTION] will still exist. */
  return ((void*) NULL);
}


/*
  Debug|Install -- install the debugger routines and tables in xxxmdbug.
  The actual CREATE statements are all xxxmdbug_install_sql in
  a separate file, install_sql.cpp, so that it's clear what the HP-licence part is.
  debug|install menu item is commented out, one must say $INSTALL
  Todo: decide what to do if it's already installed.
  Todo: privilege check
  Todo: what we really should be doing: passing the original command, in all cases.
*/
//void MainWindow::action_debug_install()
//{
//  statement_edit_widget->setPlainText("$INSTALL");
//  action_execute();
//}

void MainWindow::debug_install_go()
{
  char x[32768];
  char command_string[2048];
  QString qstring_error_message;

  if (debug_error((char*)"") != 0) return;

  qstring_error_message= debug_privilege_check(TOKEN_KEYWORD_DEBUG_INSTALL);
  if (qstring_error_message != "")
  {
    strcpy(command_string, qstring_error_message.toUtf8());
    if (debug_error(command_string) != 0) return; /* setup wouldn't be able to operate so fail */
  }


  if (debug_mdbug_install_sql(&mysql[MYSQL_MAIN_CONNECTION], x) < 0)
  {
      put_diagnostics_in_result();
      return;
    //if (debug_error((char*)"Install failed") != 0) return;
  }
  put_diagnostics_in_result();
}

/* For copyright and license notice of debug_mdbug_install function contents, see beginning of this program. */
void debug_mdbug_install()
{
  ;
}
/* End of debug_mdbug_install. End of effect of licence of debug_mdbug_install_function contents. */


/*
  Debug|Setup
  Todo: This should put up a list box with the routines which the user can execute. No hard coding of test.p8 and test.p9.
        Or, it should look in the log for the last setup by this user, and re-do it.
  Todo: Shortcut = Same as last time.
  Todo: Search for schema name not just routine name.
  Todo: We require SELECT access on mysql.proc. We could change the install_sql.cpp routines so that $setup accesses
        a view of mysql.proc, and have a WHERE clause that restricts to only some procedures.
  Format =  CALL xxxmdbug.setup('[option] object name list');
  debug|setup menu item is removed, one must say $SETUP ...
*/
//void MainWindow::action_debug_setup()
//{
//  char call_statement[512]; /* Todo: this is dangerously small. */
//  int debug_widget_index;
//  QString routine_schema_name[DEBUG_TAB_WIDGET_MAX + 1];
//  QString routine_name[DEBUG_TAB_WIDGET_MAX + 1];
//
//  /*
//    TODO: Put up a list box with the routines that the user has a right to execute.
//    Allow user to pick which ones will go in routine_names[], which will be what is used for setup().
//    Max = DEBUG_TAB_WIDGET_MAX.
//  */
//
//  routine_schema_name[0]= "test";
//  routine_schema_name[1]= "test";
//  routine_name[0]= "p8";
//  routine_name[1]= "p9";
//  routine_name[2]= "";
//
//  /* Make a setup statement. Example possibility = CALL xxxmdbug.setup('test.p8',test.p9'). */
//  strcpy(call_statement, "CALL xxxmdbug.setup('");
//  for (debug_widget_index= 0; ; ++debug_widget_index)
//  {
//    if (strcmp(routine_name[debug_widget_index].toUtf8(), "") == 0) break;
//    if (debug_widget_index > 0) strcat(call_statement, ",");
//    strcat(call_statement, routine_schema_name[debug_widget_index].toUtf8());
//    strcat(call_statement, ".");
//    strcat(call_statement, routine_name[debug_widget_index].toUtf8());
//  }
//  strcat(call_statement, "');");
//  printf("call_statement=%s\n", call_statement);
//
//  statement_edit_widget->setPlainText(call_statement);
//  emit action_execute();
//}


/* Todo: $setup does a GET_LOCK() so that two users won't do $setup at the same time. Figure out a better way. */
void MainWindow::debug_setup_go(QString text)
{
  char command_string[512];
  int index_of_number_1, index_of_number_2;
  QString qstring_error_message;
  QString q_routine_schema, q_routine_name;

  /* Todo: Check that 'install' has happened -- but for now it's OK, at least we return some sort of error. */
  /* Todo: use debug_error instead, provided debug_error gets rid of any problematic ''s */
  qstring_error_message= debug_privilege_check(TOKEN_KEYWORD_DEBUG_SETUP);
  if (qstring_error_message != "")
  {
    strcpy(command_string, qstring_error_message.toUtf8());
    if (debug_error(command_string) != 0) return; /* setup wouldn't be able to operate so fail */
  }

  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    if (debug_error(command_string) != 0) return;
  }

  q_routine_schema= debug_q_schema_name_in_statement;
  q_routine_name= debug_q_routine_name_in_statement;

  if ((q_routine_schema == "") || (q_routine_name == ""))
  {
    if (debug_error((char*)"Missing routine name(s). Expected syntax is: $setup routine-name [, routine-name ...]") != 0) return;
  }

  /* Todo: since yes there is a bug, but we want to get this out, just release the lock instead of doing the right thing. */
  {
    QString result_string= select_1_row("SELECT is_free_lock('xxxmdbug_lock')");
    if (select_1_row_result_1.toInt() != 0)
    {
      //if (debug_error((char*)"Another user has done $setup and caused a lock. This might be an ocelotgui bug.") != 0) return;
      result_string= select_1_row("SELECT release_lock('xxxmdbug_lock')");
    }
  }

  /* Throw away the first word i.e. "$setup" and execute "call xxxmdbug.setup('...')". */
  char call_statement[512 + 128];
  strcpy(call_statement, "call xxxmdbug.setup('");
  strcat(call_statement, strstr(command_string, " ") + 1);
  strcat(call_statement, "')");

  if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
  {
    put_diagnostics_in_result();
    return;
    //if (debug_error((char*)"call xxxmdbug.setup() failed.") != 0) return;
  }
  debug_setup_mysql_proc_insert();
}


/*
  The following routine replaces the procedures xxxmdbug.mysql_proc_insert()
  and xxxmdbug.mysql_proc_insert_caller() in install_sql.cpp, which are no
  longer called from xxxmdbug.setup(). The effect is the same,
  but there is no INSERT into a mysql-database table.
  Also: definition_of_surrogate_routine will have three statements: DROP IF EXISTS, SET, CREATE.
        I could execute them all at once, but prefer to separate. Assume original name didn't contain ;.
        ... For icc_core routines the SET statement might be missing, I don't know why.
*/
void MainWindow::debug_setup_mysql_proc_insert()
{
  char command[512];
  MYSQL_RES *res= NULL;
  MYSQL_ROW row= NULL;
  const char *unexpected_error= NULL;
  unsigned int num_fields;
  unsigned int num_rows;
  QString definition_of_surrogate_routine;
  int it_is_ok_if_proc_is_already_there;

  mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "select * from xxxmdbug.routines");
  res= mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
  if (res != NULL)
  {
    num_rows= mysql_num_rows(res);
    mysql_free_result(res);
    if (num_rows - 3 >= DEBUG_TAB_WIDGET_MAX)
    {
      sprintf(command, "SIGNAL SQLSTATE '05678' SET message_text='$setup generated %d surrogates but the current maximum is %d'", num_rows - 3, DEBUG_TAB_WIDGET_MAX - 1);
      mysql_query(&mysql[MYSQL_MAIN_CONNECTION], command);
      put_diagnostics_in_result();
      return;
    }
  }

  mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "SET @xxxmdbug_saved_sql_mode=@@sql_mode");
  /*
    Go through all the rows of mysql.proc twice.
    In the first iteration, we try to drop and create. If a create fails, we need a second iteration.
    In the second iteration, we try to drop but not create (this is to get rid of earlier successful creates)..
  */
  int index_of_semicolon, index_of_set, index_of_create;
  QString first_query, second_query, third_query;
  int second_iteration_is_necessary= 0;
  for (int iteration=0; iteration <= 1; ++iteration)
  {
    if ((iteration == 1) && (second_iteration_is_necessary == 0))
    {
      break;
    }
    res= NULL;
    row= NULL;

    strcpy(command,
    "SELECT TRIM(BOTH '`' FROM schema_identifier_of_original),"
    "       TRIM(BOTH '`' FROM routine_identifier_of_original),"
    "       TRIM(BOTH '`' FROM routine_identifier_of_surrogate),"
    "       definition_of_surrogate_routine,"
    "       offset_of_begin "
    "FROM xxxmdbug.routines");

    if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], command, strlen(command)))
    {
      unexpected_error= "select failed";
    }
    if (unexpected_error == NULL)
    {
      res= mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
      if (res == NULL) unexpected_error= "mysql_store_result failed";
    }
    if (unexpected_error == NULL)
    {
      num_fields= mysql_num_fields(res);
      if (num_fields != 5) unexpected_error= "wrong field count";
    }

    if (unexpected_error == NULL)
    {
      for (;;)
      {
        row= mysql_fetch_row(res);
        if (row == NULL) break;
        it_is_ok_if_proc_is_already_there= 0;
        /* todo: make sure row[2] i.e. routine_identifier_of_surrogate is not null */
        if ((strcmp(row[2], "icc_process_user_command_r_server_variables") == 0)
         || (strcmp(row[2], "icc_process_user_command_set_server_variables") == 0))
        {
          /* We will not insert icc_process_user_command_r_server_variables
             or icc_process_user_command_set_server_variables into mysql.proc
             if it already exists. This is okay as long as there is no change of
             DBMS server version or engine or plugin. But recommend always:
             DROP PROCEDURE xxxmdbug.icc_process_user_command_r_server_variables; */
          it_is_ok_if_proc_is_already_there= 1;
        }
        else
        {
          /* MDBug might not care if the routine definition was blank, but we do. */
          char tmp[512];
          MYSQL_RES *res_2= NULL;
          MYSQL_ROW row_2= NULL;
          unsigned int num_rows_2= 0;
          strcpy(tmp, "select routine_definition from information_schema.routines where routine_schema='");
          strcat(tmp, row[0]);
          strcat(tmp, "' and routine_name='");
          strcat(tmp, row[1]);
          strcat(tmp, "'");
          if (mysql_query(&mysql[MYSQL_MAIN_CONNECTION], tmp)) num_rows_2= 0;
          else
          {
            res_2= mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
            if (res_2 != NULL)
            {
              num_rows_2= mysql_num_rows(res_2);
            }
          }
          if (num_rows_2 == 1)
          {
            row_2= mysql_fetch_row(res_2);
            if ((row_2 == NULL) || (row_2[0] == NULL) || (strcmp(row_2[0],"") == 0))
            {
              char command[512];
              mysql_free_result(res_2);
              strcpy(tmp, "Could not get a routine definition for ");
              strcat(tmp, row[0]);
              strcat(tmp, ".");
              strcat(tmp, row[1]);
              strcat(tmp, ". Are you the routine creator and/or do you have SELECT privilege for mysql.proc?");
              /* Todo: merge this sort of stuff into debug_error() */
              sprintf(command, "SIGNAL sqlstate '05678' set message_text = '%s'", tmp);
              mysql_query(&mysql[MYSQL_MAIN_CONNECTION], command);
              unexpected_error= "create failed";
              put_diagnostics_in_result();
              second_iteration_is_necessary= 1;
              break;
            }
          }
          if (res_2 != NULL) mysql_free_result(res_2);
        }
        definition_of_surrogate_routine= QString::fromUtf8(row[3]);
        index_of_semicolon= definition_of_surrogate_routine.indexOf(";");
        index_of_set= definition_of_surrogate_routine.indexOf("SET session sql_mode = '", index_of_semicolon);
        if (index_of_set == -1) index_of_create= definition_of_surrogate_routine.indexOf("CREATE ", index_of_semicolon);
        else index_of_create= definition_of_surrogate_routine.indexOf("CREATE ", index_of_set);
        if (index_of_create == -1) unexpected_error= "bad definition_of_surrogate_routine";
        if (unexpected_error == NULL)
        {
          if (index_of_set == -1) first_query= definition_of_surrogate_routine.mid(0, index_of_create - 1);
          else first_query= definition_of_surrogate_routine.mid(0, index_of_set - 1);
          if (index_of_set != -1) second_query= definition_of_surrogate_routine.mid(index_of_set, index_of_create - index_of_set);
          third_query= definition_of_surrogate_routine.mid(index_of_create, -1);
          if (it_is_ok_if_proc_is_already_there == 0)
          {
            if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], first_query.toUtf8(), strlen(first_query.toUtf8())))
            {
              /* Actually, a failed drop will have to be considered to be okay. */
            }
          }
          if ((unexpected_error == NULL) && (index_of_set != -1))
          {
            if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], second_query.toUtf8(), strlen(second_query.toUtf8())))
            {
              unexpected_error= "set failed";
              break;
            }
          }
          if (unexpected_error == NULL)
          {
            if (iteration == 0)
            {
              if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], third_query.toUtf8(), strlen(third_query.toUtf8())))
              {
                /* If there's an error, the diagnostics here should go all the way back up to the user. */
                if (it_is_ok_if_proc_is_already_there == 0)
                {
                  unexpected_error= "create failed";
                  put_diagnostics_in_result();
                  second_iteration_is_necessary= 1;
                  break;
                }
              }
            }
          }
        }
      }
    }
  if (res != NULL) mysql_free_result(res);
  }

  mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "set session sql_mode=@xxxmdbug_saved_sql_mode");

  if (unexpected_error != NULL)
  {
    if (strcmp(unexpected_error, "create failed") ==0) return; /* we already have the diagnostics */
    sprintf(command, "SIGNAL SQLSTATE '05678' SET message_text='%s'", unexpected_error);
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], command);
  }
  put_diagnostics_in_result();
}


/*
  debug_privilege_check() -- check in advance whether some debuggee-related privileges exist
  It will be more convenient if the user is told in advance, and is told multiple problems at once.
  Of course I could search for the grants, but it's easier to try it and see what happens.
  However, this doesn't consider all possibilities and is only called before initializing.
  To see if you have execute privilege, see if you can select from information_schema.routines, rather than execute.
  A variant of that would be selecting where routine_definition > '' if you want to look for all privileges.
  As well as the checks here, $setup also needs routine_definition > '' i.e. you must be routine creator
  or you must have select privilege on mysql.proc.
  Todo: check other possible things that the debuggee could do, and check all routines not just xxxmdbug.command.
        one way would be to say 'command' OR 'other_routine' ... but putting into a loop would be better, I think.
  Todo: xxxmdbug.command calls xxxmdbug.privilege_checks which will generate a signal,
        but I'm wondering whether it's good because it sends messages using init_connect every time.
  statement_type = TOKEN_KEYWORD_DEBUG_DEBUG or TOKEN_KEYWORD_DEBUG_SETUP or TOKEN_KEYWORD_DEBUG_INSTALL.
*/
QString MainWindow::debug_privilege_check(int statement_type)
{
  QString s= "";
  char call_statement[512];
  unsigned int num_rows;
  MYSQL_RES *res;
  QString result_string;

  if (is_mysql_connected != 1)
  {
    s.append("Not connected");
    return s;
  }

  if (statement_type == TOKEN_KEYWORD_DEBUG_INSTALL)
  {
    /* Somebody has to have said: grant create, drop, create routine, alter routine, select, insert, update, delete, select on xxxmdbug.* ... */
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "create database xxxmdbug");
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need create privilege on xxxmdbug.*. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "drop table if exists xxxmdbug.xxxmdbug");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "create table xxxmdbug.xxxmdbug (s1 int)");
    if ((s == "") && (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142)) s.append("Need create privilege on xxxmdbug.*. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'select * from xxxmdbug.xxxmdbug'");
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need select privilege on xxxmdbug.*. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'insert into xxxmdbug.xxxmdbug values (1)'");
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need insert privilege on xxxmdbug.*. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'delete from xxxmdbug.xxxmdbug'");
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need delete privilege on xxxmdbug.*. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'update xxxmdbug.xxxmdbug set s1 = 1'");
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need update privilege on xxxmdbug.*. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "drop table xxxmdbug.xxxmdbug");
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need drop privilege on xxxmdbug.*. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "drop procedure if exists xxxmdbug.xxxmdbug");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "create procedure xxxmdbug.xxxmdbug () set @a = 1");
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need create routine privilege on xxxmdbug.*. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "drop procedure xxxmdbug.xxxmdbug");
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1370) s.append("Need alter routine privilege on xxxmdbug.*. ");
    return s;
  }

  /* First make sure xxxmdbug exists */
  num_rows= 0;
  if (mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "select * from information_schema.schemata where schema_name = 'xxxmdbug'"))
  {
    s.append("Cannot select from information_schema");
    return s;
  }
  res= mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
  if (res != NULL)
  {
    num_rows= mysql_num_rows(res);
    mysql_free_result(res);
  }
  if (num_rows != 1)
  {
    s.append("Cannot access database xxxmdbug. Check that $install was done and privileges were granted.");
    return s;
  }

  if ((statement_type == TOKEN_KEYWORD_DEBUG_DEBUG) || (statement_type == TOKEN_KEYWORD_DEBUG_SETUP))
  {
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'select * from xxxmdbug.readme union all select * from xxxmdbug.copyright'");/* Returns 1142 if SELECT denied */
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need select privilege on xxxmdbug.*. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'create temporary table xxxmdbug.xxxmdbug_tmp (s1 int)'");/* Returns 1044 if CREATE TEMPORARY denied */
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need create temporary privilege on xxxmdbug.*. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
  }

  if (statement_type == TOKEN_KEYWORD_DEBUG_DEBUG)
  {
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'set global init_connect = @@init_connect'");/* Returns 1227 if you need SUPER */
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1227) s.append("Need super privilege. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'call xxxmdbug.command()'");
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need execute privilege on xxxmdbug.command. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
  }

  if (statement_type == TOKEN_KEYWORD_DEBUG_SETUP)
  {
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare stmt1 from 'select * from mysql.proc'");
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1142) s.append("Need select privilege on mysql.proc. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");

    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'create table xxxmdbug.xxxmdbug_nontmp (s1 int)'");/* Returns 1044 if CREATE TEMPORARY denied */
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need create privilege on xxxmdbug.*. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");

    /* Can't prepare create-routine statements so test by actually creating and dropping a trivial for-test-only routine */
    sprintf(call_statement, "create procedure xxxmdbug.xxxmdbugp () set @=@a");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement);
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need create routine privilege on xxxmdbug.*. ");
    else
    {
      sprintf(call_statement, "drop procedure xxxmdbug.xxxmdbugp");
      mysql_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement);
      if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need drop routine privilege on xxxmdbug.*. ");
    }
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'call xxxmdbug.setup()'");
    if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need execute privilege on xxxmdbug.setup. ");
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");

    result_string= select_1_row("select count(*) from information_schema.tables where table_schema = 'xxxmdbug' and table_name = 'setup_log'" );
    if (result_string != "") s.append(result_string);
    else if (select_1_row_result_1.toInt() != 0)
    {
      mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'select * from xxxmdbug.setup_log'");
      if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need select privilege on xxxmdbug.setup_log. ");
      mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
      mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "prepare xxxmdbug_stmt from 'insert into setup_log select * from xxxmdbug.setup_log'");
      if (mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]) == 1044) s.append("Need insert privilege on xxxmdbug.setup_log. ");
      mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "deallocate prepare xxxmdbug_stmt");
      /* Todo: Find out why repair table is sometimes necessary. I know it can get corrupted but don't yet know why. */
      if (mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "repair table xxxmdbug.setup_log"))
      {
        s.append("Repair Table xxxmdbug.setup_log failed");
      }
      else
      {
        res= mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
        if (res != NULL) mysql_free_result(res);
      }
    }
  }

  /* TODO: Following isn't good. */

  result_string= select_1_row("select count(*) from information_schema.routines where routine_schema = 'xxxmdbug' and routine_name = 'command'" );
  if (result_string != "") s.append(result_string);
  else if (select_1_row_result_1.toInt() == 0) s.append("Need execute privilege for xxxmdbug.*. ");
  return s;
}


/*
  Called from: debug_debug_go(), debug_breakpoint_or_clear_go(), or other debug_ routines that might contain arguments.
  An SQL statement may contain comments, plus semicolon and/or delimiter, which should be stripped before passing to xxxmdbug.
  Also these procedures need to know: which tokens (if any) are for schema, routine, start line number, end line number.
  For example from "$DEBUG x.y -- comment;", we want to produce "debug x.y" and the offsets of x and y.
  Possible are: breakpoint/tbreakpoint location information [condition information]
                clear [schema_identifier[.routine_identifier]] line_number_minimum [-line_number_maximum]
                debug [schema_identifier[.routine_identifier]
                delete breakpoint number
  This does not detect syntax errors, we assume xxxmdbug.command() will return an error immediately for a syntax error.
  So it's not really a parse, but should find the tokens that matter.
  Todo: see if confusion happens if there's condition information, or parameters enclosed inside ''s.
  If there is clearly an error, debug_parse_statement() returns -1 and command_string has an error message.
*/
int MainWindow::debug_parse_statement(QString text,
                           char *command_string,
                           int *index_of_number_1,
                           int *index_of_number_2)
{
  char token[512];
  int i, s_len;
  int token_type;
  QString s;
//  int index_of_dot= -1;
//  int index_of_minus= -1;
  int statement_type= 0;
  //bool left_parenthesis_seen= false;
  bool name_is_expected= false; /* true if syntax demands [schema_name.]routine_name at this point */
  char default_schema_name[512];

  strcpy(default_schema_name, "");
  debug_q_schema_name_in_statement="";
  debug_q_routine_name_in_statement="";
  *index_of_number_1= -1;
  *index_of_number_2= -1;
  strcpy(command_string, "");
  for (i= 0; main_token_lengths[i] != 0; ++i)
  {
    token_type= main_token_types[i];
    if ((token_type == TOKEN_TYPE_COMMENT_WITH_SLASH)
     || (token_type == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     || (token_type == TOKEN_TYPE_COMMENT_WITH_MINUS)) continue;
    s= text.mid(main_token_offsets[i], main_token_lengths[i]);

    if ((token_type >= TOKEN_KEYWORD_DEBUG_BREAKPOINT)
     && (token_type <= TOKEN_KEYWORD_DEBUG_TBREAKPOINT)
     && (strcmp(command_string, "") == 0))
    {
      /* We're at the first word, which is the statement type. Write it in full. */
      statement_type= token_type;
      if (token_type == TOKEN_KEYWORD_DEBUG_BREAKPOINT) { strcpy(command_string, "breakpoint"); name_is_expected= true; }
      if (token_type == TOKEN_KEYWORD_DEBUG_CLEAR) { strcpy(command_string, "clear"); name_is_expected= true; }
      if (token_type == TOKEN_KEYWORD_DEBUG_CONTINUE) strcpy(command_string, "continue");
      if (token_type == TOKEN_KEYWORD_DEBUG_DEBUG) { strcpy(command_string, "debug"); name_is_expected= true; }
      if (token_type == TOKEN_KEYWORD_DEBUG_DELETE) strcpy(command_string, "delete");
      if (token_type == TOKEN_KEYWORD_DEBUG_EXECUTE) strcpy(command_string, "execute");
      if (token_type == TOKEN_KEYWORD_DEBUG_EXIT) strcpy(command_string, "exit");
      if (token_type == TOKEN_KEYWORD_DEBUG_INFORMATION) strcpy(command_string, "information");
      if (token_type == TOKEN_KEYWORD_DEBUG_INSTALL) strcpy(command_string, "install");
      if (token_type == TOKEN_KEYWORD_DEBUG_LEAVE) strcpy(command_string, "leave");
      if (token_type == TOKEN_KEYWORD_DEBUG_NEXT) strcpy(command_string, "next");
      if (token_type == TOKEN_KEYWORD_DEBUG_REFRESH) strcpy(command_string, "refresh");
      if (token_type == TOKEN_KEYWORD_DEBUG_SET) strcpy(command_string, "set");
      if (token_type == TOKEN_KEYWORD_DEBUG_SETUP) { strcpy(command_string, "setup"); name_is_expected= true; }
      if (token_type == TOKEN_KEYWORD_DEBUG_SKIP) strcpy(command_string, "skip");
      if (token_type == TOKEN_KEYWORD_DEBUG_SOURCE) strcpy(command_string, "source");
      if (token_type == TOKEN_KEYWORD_DEBUG_STEP) strcpy(command_string, "step");
      if (token_type == TOKEN_KEYWORD_DEBUG_TBREAKPOINT) { strcpy(command_string, "tbreakpoint"); name_is_expected= true; }
      continue;
    }

    s= text.mid(main_token_offsets[i], main_token_lengths[i]);
    s_len= s.toUtf8().size(); /* See comment "UTF8 Conversion" */
    if (s_len + 1 >= 512) { strcpy(command_string, "overflow"); return -1; }
    memcpy(token, s.toUtf8().constData(), s_len);
    token[s_len]= 0;
    if (token[0] == ';') continue;
    if (s == ocelot_delimiter_str) continue;
//    if (token[0] == '.') index_of_dot= i;
//    if (token[0] == '-') index_of_minus= i;
//    if (token[0] == '(') left_parenthesis_seen= true;
    if (name_is_expected)
    {
      if ((token_type == TOKEN_TYPE_OTHER)
       || (token_type == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK))
      {
        /* It's possibly a routine name, and we're expecting a routine name. */
        char tmp_schema_name[512];
        char tmp_routine_name[512];
        int s2_len;
        QString s2;
        if (text.mid(main_token_offsets[i + 1], 1) == ".")
        {
          /* schema-name . routine_name */
          if ((main_token_types[i + 2] == TOKEN_TYPE_OTHER)
           || (main_token_types[i + 2] == TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK))
          {
            strcpy(tmp_schema_name, token);
            s2= text.mid(main_token_offsets[i + 2], main_token_lengths[i + 2]);
            s2_len= s2.toUtf8().size(); /* See comment "UTF8 Conversion" */
            if (s2_len + 1 + s_len + 1 >= 512) { strcpy(command_string, "overflow"); return -1; }
            memcpy(tmp_routine_name, s2.toUtf8().constData(), s2_len);
            tmp_routine_name[s2_len]= 0;
            i+= 2;
          }
          else
          {
            strcpy(command_string, "Expected routine-name after .");
            return -1;
          }
        }
        else
        {
          /* Just routine name. Fill in [default schema name]. Todo: shouldn't require a server request so frequently. */
          if (strcmp(default_schema_name, "") == 0)
          {
            QString result_string;
            select_1_row_result_1= "";
            result_string= select_1_row("select database()");
            if (result_string != "") { strcpy(command_string, "Need to know default database but select database() failed"); return -1; }
            if (select_1_row_result_1== "") { strcpy(command_string, "Need to know default database but select database() returned nothing"); return -1; }
            s2= select_1_row_result_1;
            s2_len= s2.toUtf8().size();
            if (s2_len + 1 + s_len + 1 >= 512) { strcpy(command_string, "overflow"); return -1; }
            memcpy(default_schema_name, s2.toUtf8().constData(), s2_len);
            default_schema_name[s2_len]= 0;
          }
          strcpy(tmp_routine_name, token);
          strcpy(tmp_schema_name, default_schema_name);
        }
        name_is_expected= false;
        strcpy(token, tmp_schema_name);
        strcat(token, ".");
        strcat(token, tmp_routine_name);

        {
          QString result_string;
          char query_for_select_check[512];
          sprintf(query_for_select_check, "select count(*) from information_schema.routines where routine_schema='%s' and routine_name='%s'", tmp_schema_name, tmp_routine_name);
          result_string= select_1_row(query_for_select_check);
          if (result_string != "") { strcpy(command_string, "Tried to check routine name but select from information_schema.routines failed"); return -1; }
          if (select_1_row_result_1.toInt() == 0)
          {
            sprintf(command_string, "Could not find routine %s.%s", tmp_schema_name, tmp_routine_name);
            return -1;
          }

        }

        debug_q_schema_name_in_statement= tmp_schema_name;
        debug_q_routine_name_in_statement= tmp_routine_name;
      }
      else
      {
        /* It's not possibly a routine identifier, and we're expecting a routine identifier. */
        strcpy(command_string, "Expected a routine name"); return -1;
      }
    }
    if ((token[0] == ',') && (statement_type == TOKEN_KEYWORD_DEBUG_SETUP)) name_is_expected= true;

    if (token_type == TOKEN_TYPE_LITERAL_WITH_DIGIT)
    {
      if (*index_of_number_1 == -1) *index_of_number_1= i;
      else if (*index_of_number_2 == -1) *index_of_number_2= i;
    }
    if (strlen(command_string) + strlen(token) + 1 >= 512) { strcpy(command_string, "overflow"); return -1; }
    if (strcmp(command_string, "") != 0) strcat(command_string, " ");
    strcat(command_string, token);
  }
  return 0;
}


/*
  First: find the surrogate routine that debuggee will call (if it's not there, error = you should call setup).
  Todo: schema_name could be '.' i.e. default
  Then: put up debug_top_widget
  Then: execute xxxmdbug.command(...,'debug ...')
  Todo: check that surrogate routine is not obsolete, i.e. timestamp is after original routine's timestamp
  Todo: check that everything in the group is present and executable
  Todo: check that user is the same ... but how can you do that? Is it only in setup_log?
  Todo: get rid of initialize() from menu
  Todo: when starting, make sure debuggee is not already running -- even a finished routine is not over
  Todo: if clicking causes you to get to action_debug_debug() you must generate a $DEBUG statement -- but how click a list??
*/
//void MainWindow::action_debug_debug()
//{
//  /* Figure out what the parameter should be -- it's not text, eh? */
//  QString parameter;
//  debug_debug_go(parameter);
//}


/*
  Call debug_error((char*)"error-text") whenever, in one of the debug routines, you encounter an error.
  Call debug_error((char*)"") regardless, at the start of a debug routine, so basic prerequisite checks can happen.
  For example, if the user says "$debug test.x" and there is no surrogate for x, we want the user to see:
  Error 1644 (05678) Surrogate not found.
  TODO: make sure all the debug-routine failures go through here!
  Todo: if the error is due to a failure of the last call, add what that last call's error message was.
*/
int MainWindow::debug_error(char *text)
{
  char command[2048];

//  unsigned int statement_type;

  if (is_mysql_connected == 0)
  {
    /* Unfortunately this might not get through. */
    statement_edit_widget->result= tr("ERROR not connected");
    return 1;
  }

  if (debuggee_state < 0)
  {
    char tmp_string[STRING_LENGTH_512]="";
    char tmp_string_2[STRING_LENGTH_512 + 64];
    if (debuggee_state != DEBUGGEE_STATE_END)
    {
      /* debuggee_state_error should tell us something, but remove all ' first */
      strcpy(tmp_string, debuggee_state_error);
      for (int i= 0; tmp_string[i] != '\0'; ++i) if (tmp_string[i] == '\047') tmp_string[i]= ' ';
    }
    sprintf(tmp_string_2, "SIGNAL sqlstate '05678' set message_text = '%s %d. %s'", "debuggee_state:", debuggee_state, tmp_string);
    if (debuggee_state == DEBUGGEE_STATE_END)
    {
      sprintf(tmp_string_2, "SIGNAL sqlstate '05678' set message_text = 'Routine has stopped. Suggested next step is: $EXIT'");
    }
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], tmp_string_2);
    put_diagnostics_in_result();
    return 1;
  }


  if (strcmp(text,"") != 0)
  {
    sprintf(command, "SIGNAL sqlstate '05678' set message_text = '%s'", text);
    mysql_query(&mysql[MYSQL_MAIN_CONNECTION], command);
    put_diagnostics_in_result();
    return 1;
  }

  return 0;
}


/* $DEBUG [schema.]routine [(parameters)] */
void MainWindow::debug_debug_go(QString text) /* called from execute_client_statement() or action_debug_debug() */
{
  char routine_schema[512];
  char routine_name[512];
  char call_statement[512];
  QString result_string;
  QString qstring_error_message;
  int current_widget_index;
  QString q_routine_schema, q_routine_name;
  char command_string[2048];
  int index_of_number_1, index_of_number_2;

  /* Todo: Check that 'debug' has not happened */

  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    if (debug_error(command_string) != 0) return;
  }

  q_routine_schema= debug_q_schema_name_in_statement;
  q_routine_name= debug_q_routine_name_in_statement;

  if ((q_routine_schema == "") || (q_routine_name == ""))
  {
    if (debug_error((char*)"Missing routine name") != 0) return;
  }

  /* This is in case an error left tab widget up. Todo: Examine if other debuggee errors should get the same treatment. */
  if (debuggee_state == DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR)
  {
    debug_exit_go(1);
  }

  if (debuggee_state < 0) debuggee_state= DEBUGGEE_STATE_0;
  if (debuggee_state == DEBUGGEE_STATE_END) debuggee_state= DEBUGGEE_STATE_0;
  if (debug_error((char*)"") != 0) return;


  strcpy(routine_schema, q_routine_schema.toUtf8());
  strcpy(routine_name, q_routine_name.toUtf8());

  if (debuggee_state > 0)
  {
    if (debug_error((char*)"Debug is already running. Use Debug|exit to stop it. This could be temporary.") != 0) return;
  }

  qstring_error_message= debug_privilege_check(TOKEN_KEYWORD_DEBUG_DEBUG);
  if (qstring_error_message != "")
  {
    strcpy(command_string, qstring_error_message.toUtf8());
    if (debug_error(command_string) != 0) return; /* debuggee wouldn't be able to operate so fail */
  }

  /* Call xxxmdbug.debuggee_get_surrogate_name */
  strcpy(call_statement, "CALL xxxmdbug.debuggee_get_surrogate_name('");
  strcat(call_statement, routine_name);
  strcat(call_statement, "','");
  strcat(call_statement, routine_schema);
  strcat(call_statement, "',@schema_identifier,@surrogate_routine_identifier,@routine_type,@remainder_of_original_name)");
  if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
  {
    if (debug_error((char*)"Surrogate not found. Probably $setup wasn't done for a group including this routine.") != 0) return;
  }
  result_string= select_1_row("select @schema_identifier, concat(left(@surrogate_routine_identifier,11), '%'), @routine_type, @remainder_of_original_name, isnull(@schema_identifier)");
  if (result_string != "")
  {
    char char_result_string[512];
    strcpy(char_result_string, result_string.toUtf8());
    if (debug_error(char_result_string) != 0) return;
  }
  /* select_1_row_result_1 = @schema_identifier. select_1_row_result_2 = left(@surrogate_routine_identifier,11) || '%'. */

  /* Todo: this checks whether select returned null, good. But it should make even more certain that $debug will succeed. */
  if (select_1_row_result_5.toInt() == 1)
  {
    if (debug_error((char*)"Surrogate not found. Perhaps $setup was not done?") != 0) return;
  }

  QString routine_schema_name_for_search= select_1_row_result_1;
  QString routine_name_for_search= select_1_row_result_2;

  /*
    This will find all surrogate routines which have the same prefix, xxxmdbug___
    That means they're in the same group. Ignore icc_core.
    Get only the part that's not part of the prefix.
    Interesting idea: you could have a way to show both the surrogate and the original.
  */
  int i, j;
  char i_as_string[10];
  /* Todo: n should not be hard-coded here, it limits us to only 10 routines can be searched */
  for (i= 0, j= 0; i < DEBUG_TAB_WIDGET_MAX; ++i)
  {
    sprintf(i_as_string, "%d", i);
    strcpy(call_statement, "select routine_schema, right(routine_name,length(routine_name)-12) from information_schema.routines ");
    strcat(call_statement, "where routine_schema = '");
    strcat(call_statement, routine_schema_name_for_search.toUtf8());
    strcat(call_statement, "' and routine_name like '");
    strcat(call_statement, routine_name_for_search.toUtf8());
    strcat(call_statement, "' order by routine_name limit 1 offset ");
    strcat(call_statement, i_as_string);
    result_string= select_1_row(call_statement);
    if (result_string != "")
    {
      char char_result_string[512];
      if (result_string == "mysql_fetch row failed") break;
      strcpy(char_result_string, result_string.toUtf8());
      if (debug_error(char_result_string) != 0) return;
    }
    if (select_1_row_result_2 != "icc_core")
    {
      /* Todo: debug_routine_schema_name and debug_routine_name are global so setup() will ruin them. Fix that! */
      debug_routine_schema_name[j]= select_1_row_result_1;
      debug_routine_name[j]= select_1_row_result_2;
      ++j;
    }
  }

  /* Todo: check: can this crash if there are DEBUG_TAB_WIDGET_MAX routines? Is this unnecessary if there are DEBUG_TAB_WIDGET_MAX routines? */
  debug_routine_name[j]= "";

  int debug_widget_index;

  /* Todo: check if this is useless. You're supposed to have called debug_delete_tab_widgets[] during $exit. */
  for (debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    debug_widget[debug_widget_index]= 0;
  }

  /* After this point, some items that get created are persistent, so be sure to clear them if there's an error. */

  //MYSQL_RES *debug_res;
  //MYSQL_ROW row;
  QString routine_definition;
  current_widget_index= -1;
  for (debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    if (strcmp(debug_routine_name[debug_widget_index].toUtf8(), "") == 0) break;
    strcpy(call_statement, "select routine_definition from information_schema.routines where routine_schema = '");
    strcat(call_statement, debug_routine_schema_name[debug_widget_index].toUtf8());
    strcat(call_statement, "' and routine_name = '");
    strcat(call_statement, debug_routine_name[debug_widget_index].toUtf8());
    strcat(call_statement, "'");
    QString error_return= select_1_row(call_statement);
    if (error_return != "")
    {
      debug_delete_tab_widgets();                          /* delete already-made widgets. Rest of 'exit' shouldn't happen. */
      strcpy(call_statement, error_return.toUtf8());
      strcat(call_statement, " Could not find a routine in the $setup group: ");
      strcat(call_statement, debug_routine_schema_name[debug_widget_index].toUtf8());
      strcat(call_statement,".");
      strcat(call_statement, debug_routine_name[debug_widget_index].toUtf8());
      debug_error(call_statement);                         /* Todo: you forgot to look for icc_core */
      return;
    }
    routine_definition= select_1_row_result_1;             /* = information_schema.routines.routine_definition */

    if (routine_definition == "")
    {
      debug_delete_tab_widgets();
      strcpy(call_statement, "Could not get a routine definition for ");
      strcat(call_statement, debug_routine_name[debug_widget_index].toUtf8());
      strcat(call_statement, ". Are you the routine creator and/or do you have SELECT privilege for mysql.proc?");
      if (debug_error(call_statement)) return;
    }

    debug_widget[debug_widget_index]= new CodeEditor();
    debug_widget[debug_widget_index]->statement_edit_widget_left_bgcolor= QColor(ocelot_statement_prompt_background_color);
    debug_widget[debug_widget_index]->statement_edit_widget_left_treatment1_textcolor= QColor(ocelot_statement_color);

    debug_maintain_prompt(0, debug_widget_index, 0); /* clear prompt_as_input_by_user */

    /* todo: call 'refresh breakpoints' and debug_maintain_prompt() for breakpoints that already have been set up. */
    debug_widget[debug_widget_index]->prompt_default= (QString)"\\2 \\L";
    debug_widget[debug_widget_index]->result= (QString)"ABCDEFG";
    debug_widget[debug_widget_index]->setPlainText(routine_definition);

    debug_widget[debug_widget_index]->setReadOnly(false);                 /* if debug shouldn't be editable, set to "true" here */
    debug_widget[debug_widget_index]->installEventFilter(this);           /* is this necessary? */
    debug_tab_widget->addTab(debug_widget[debug_widget_index], debug_routine_name[debug_widget_index]);

    if ((QString::compare(debug_routine_name[debug_widget_index], q_routine_name, Qt::CaseInsensitive) == 0)
    &&  (QString::compare(debug_routine_schema_name[debug_widget_index], q_routine_schema, Qt::CaseInsensitive) == 0))
    {
      current_widget_index= debug_widget_index;
    }
  }

  if (current_widget_index == -1)
  {
    debug_delete_tab_widgets();
    if (debug_error((char*)"routine is missing")) return;
  }

  debug_tab_widget->setCurrentIndex(current_widget_index);

  /* Getting ready to create a separate thread for the debugger and 'attach' to it */

  char error_message[512];

  debuggee_state= DEBUGGEE_STATE_0;

  /*
    We want a channel name that will differ from what others might choose.
    Arbitrary decision = debug_channel_name will be "ch#" || connection-number-of-main-connection.
    It could be uniquified beyond that if necessary, e.g. add user name.
    Todo: this could be moved so it doesn't happen every time $debug happens. */

  sprintf(debuggee_channel_name, "ch#%d", statement_edit_widget->dbms_connection_id);

  pthread_t thread_id;

  /* Create a debuggee thread. */
  /* Todo: consider whether it would have been better to use the Qt QThread function */
  pthread_create(&thread_id, NULL, &debuggee_thread, NULL);

  /*
    Wait till debuggee has indicated that it is about to call debuggee_wait_loop().
    Todo: Give a better diagnostic if this doesn't happen.
    Todo: Is it possible to fail anyway? If so, sleep + repeat?
    Todo: This gives up after 1 second. Maybe on a heavily loaded machine that's too early?
  */
  for (int k= 0; k < 100; ++k)
  {
    QThread48::msleep(10);
    if (debuggee_state == DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP) break;
  }
  QThread48::msleep(10); /* in case debuggee_wait_loop() fails immediately */
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    /* Todo: if somehow (bizarrely) debuggee_state >= 0, then the thread did not end and needs to be stopped. */
    debug_delete_tab_widgets();
    sprintf(error_message, "Debuggee not responding. Code = %d. Thread has not been stopped.\n", debuggee_state);
    if (debug_error(error_message) != 0) return;
  }
  /*
    Attach to the debuggee.
    Todo: Check: is it possible for this to fail because thread has not connected yet? If so, sleep + repeat?
    Todo: Check: why use insertPlainText not setPlainText?
  */

  strcpy(call_statement, "call xxxmdbug.command('");
  strcat(call_statement, debuggee_channel_name);
  strcat(call_statement, "', 'attach');\n");

  if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
  {
    /* Attach failed. Doubtless there's some sort of error message. Put it out now, debug_exit_go() won't override it. */
    put_diagnostics_in_result();
    debug_exit_go(1); /* This tries to tell the debuggee to stop, because we're giving up. */
    return;
  }

  debug_timer_old_line_number= -1;
  debug_timer_old_schema_identifier[0]= '\0';
  debug_timer_old_routine_identifier[0]= '\0';
  debug_timer_old_commands_count= -1;
  debug_timer_old_number_of_status_message= -1;
  debug_timer_old_icc_count= -1;
  debug_timer_old_debug_widget_index= -1;
  debug_timer->start(DEBUG_TIMER_INTERVAL);

  /* By the way, we fill in [schema.] if it's missing, because otherwise default would be 'test'. */

  //  strcpy(call_statement, "debug ");
  //if (strcmp(routine_schema, "") != 0)
  //{
  //  strcat(call_statement, routine_schema);
  //  strcat(call_statement, ".");
  //}
  //strcat(call_statement, routine_name);

  /*
    To work around a flaw in MDBug, convert all ' to ''.
    Todo: This does not solve for $debug var_proc(''a''bc'');
    so the manual should recommend: use an escape for '.
    Todo: Check if other debug commands containing ', e.g. conditional breakpoints, work.
  */
  if (strchr(command_string, 0x27) != NULL)
  {
    char command_string_2[2048];
    char *iptr, *optr;
    for (iptr= command_string, optr= command_string_2;;)
      {
      if (*iptr == 0x27) { *optr= 0x27; ++optr; }
      *optr= *iptr;
      if (*iptr == '\0') break;
      ++iptr;
      ++optr;
    }
    strcpy(command_string, command_string_2);
  }

  if (debug_call_xxxmdbug_command(command_string) != 0)
  {
    /* Debug failed. Doubtless there's some sort of error message. Put it out now, debug_exit_go() won't override it. */
    put_diagnostics_in_result();
    debug_exit_go(1); /* This tries to tell the debuggee to stop, because we're giving up. */
    return;
  }
  put_diagnostics_in_result();

  /* TODO: next line was removed TEMPORARILY. But maybe highlighting will occur due to temporary breakpoint? */
  //debug_highlight_line(); /* highlight the first line. todo: should be the first executable line, no? */
  debug_top_widget->show(); /* Is this necessary? I think so because initially debug_window should be hidden. */

  debug_menu_enable_or_disable(TOKEN_KEYWORD_DEBUG_DEBUG);             /* Until now most debug menu items were gray. */
}


/*
  Change the contents of debug_widget[debug_widget_index]->prompt_as_input_by_user.
  action: 0 = initialize/clear, 1 = add breakpoint, 2 = delete breakpoint, 3 = add tbreakpoint
  line_number: irrelevant if action == 0, else line number for add/clear
  We have to set the prompt -- the separated-from-the-text stuff on the left -- so that it shows
  the line numbers with \2 \L, and shows the breakpoints with K(line-number).
  We avoid 'refresh breakpoints' because it waits for debuggee to be free; hope it doesn't get out of synch.
  We should end up with something like "\2 \L \K(5,red,Ⓑ) \K(4,red,Ⓑ)" if there are breakpoints on line 5 and line 4.
  Todo: Even if we're doing adds, do all the deletes first so that if this is called twice there won't be a duplicate.
        (Todo: I think the debuggee would not make duplicates, but that needs checking.)
  Todo: breakpoint and tbreakpoint look the same in this scheme.
*/
void MainWindow::debug_maintain_prompt(int action, int debug_widget_index, int line_number)
{
  if (action == 0)                                                         /* clear */
  {
    debug_widget[debug_widget_index]->prompt_as_input_by_user= (QString)"\\2 \\L";
    return;
  }
  QString prompt_including_symbol;
  QString breakpoint_symbol;

  prompt_including_symbol= (QString)" \\K(";
  prompt_including_symbol.append(QString::number(line_number));
  breakpoint_symbol= QChar(0x24b7);                                        /* CIRCLED LATIN CAPITAL LETTER B */
  prompt_including_symbol.append(",red,");
  prompt_including_symbol.append(breakpoint_symbol);
  prompt_including_symbol.append(")");

  if (action == 2)                                                        /* delete (well, actually: clear */
  {
    /* I don't expect that indexOf() could return -1, and I ignore it if it does happen. */
    int index_of_prompt;
    index_of_prompt= debug_widget[debug_widget_index]->prompt_default.indexOf(prompt_including_symbol);
    if (index_of_prompt >= 0) debug_widget[debug_widget_index]->prompt_default.remove(index_of_prompt, prompt_including_symbol.size());
    index_of_prompt= debug_widget[debug_widget_index]->prompt_as_input_by_user.indexOf(prompt_including_symbol);
    if (index_of_prompt >= 0) debug_widget[debug_widget_index]->prompt_as_input_by_user.remove(index_of_prompt, prompt_including_symbol.size());
  }
  if ((action == 1) || (action == 3))                                    /* add */
  {
    debug_widget[debug_widget_index]->prompt_default.append(prompt_including_symbol);
    debug_widget[debug_widget_index]->prompt_as_input_by_user.append(prompt_including_symbol);
  }
}


/*
  Find out what is current routine and current line, put a symbol beside it.
  Execute a command e.g. "debug test.p8 5".
  So far: we get block_number = current line number - 1.
  Todo: we don't show the B in a circle!
  Do not confuse with action_mousebuttonpress which also generates "$breakpoint ...".
*/
void MainWindow::action_debug_breakpoint()
{
  char command[512];
  int line_number;
  char line_number_as_string[10];

  int debug_widget_index= debug_tab_widget->currentIndex();
  /* I can't imagine how currentIndex() could be == -1, but if it is, do nothing. */
  if (debug_widget_index < 0) return;
  line_number= debug_widget[debug_widget_index]->block_number + 1;
  sprintf(line_number_as_string, "%d", line_number);
  strcpy(command, "$BREAKPOINT ");
  strcat(command, debug_routine_schema_name[debug_widget_index].toUtf8());
  strcat(command,".");
  strcat(command, debug_routine_name[debug_widget_index].toUtf8());
  strcat(command, " ");
  strcat(command, line_number_as_string);

  statement_edit_widget->setPlainText(command);
  action_execute();
}


/*
  "breakpoint" and "clear" and "tbreakpoint" have the same argument syntax so we use the same routine for all,
  passing statement_type == TOKEN_KEYWORD_DEBUG_BREAKPOINT or TOKEN_KEYWORD_DEBUG_CLEAR.
*/
void MainWindow::debug_breakpoint_or_clear_go(int statement_type, QString text)
{
  char command_string[512];
  int index_of_number_1, index_of_number_2;
  QString routine_name;
  QString schema_name;

  /* Todo: Check that 'debug' has happened */
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }

  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    if (debug_error((char*)"Overflow") != 0) return;
  }
  schema_name= debug_q_schema_name_in_statement;
  routine_name= debug_q_routine_name_in_statement;

  if ((schema_name == "") || (routine_name == ""))
  {
    if (debug_error((char*)"Missing routine name") != 0) return;
  }

  if (debug_error((char*)"") != 0) return;

  if (debug_call_xxxmdbug_command(command_string) != 0)
  {
    put_diagnostics_in_result();
    return;
  }

  /* Figure out line number and debug_widget_index from the index variables. */
  /* Todo: This is failing to compare schema name. */

  int debug_widget_index;

  for (debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    if (debug_widget[debug_widget_index] != 0)
    {
      if ((QString::compare(routine_name, debug_routine_name[debug_widget_index]) == 0)
       && (QString::compare(schema_name, debug_routine_schema_name[debug_widget_index]) == 0)) break;
    }
  }

  if (debug_widget_index == DEBUG_TAB_WIDGET_MAX)
  {
    if (debug_error((char*)"No such routine") != 0) return;
  }

  int line_number_1, line_number_2;
  QString q_line_number;
  q_line_number= text.mid(main_token_offsets[index_of_number_1], main_token_lengths[index_of_number_1]);
  line_number_1= q_line_number.toInt();
  if (index_of_number_2 == -1) line_number_2= line_number_1;
  else
  {
    q_line_number= text.mid(main_token_offsets[index_of_number_2], main_token_lengths[index_of_number_2]);
    line_number_2= q_line_number.toInt();
  }

  for (int i= line_number_1; i <= line_number_2; ++i)
  {
    if (i > debug_widget[debug_widget_index]->blockCount())
    {
      break;
    }
    if (statement_type == TOKEN_KEYWORD_DEBUG_BREAKPOINT) debug_maintain_prompt(1, debug_widget_index, i);
    if (statement_type == TOKEN_KEYWORD_DEBUG_CLEAR) debug_maintain_prompt(2, debug_widget_index, i);
    if (statement_type == TOKEN_KEYWORD_DEBUG_TBREAKPOINT) debug_maintain_prompt(3, debug_widget_index, i);
  }
  /*
    Todo: I have had trouble, sometimes the breakpoint symbols don't show up unless I click on the line.
          I think that one of these kludges is helping, but I don't know if it always helps,
          and I don't know which of the two lines is actually fixing the problem,
          and I don't know whether there's a better way. So find out, eh?
  */
  debug_widget[debug_widget_index]->repaint();
  debug_widget[debug_widget_index]->viewport()->update();
  put_diagnostics_in_result();
}


/*
  Todo: The problem with "delete n" is we don't know which breakpoint is n.
  Of course it's in xxxmdbug.breakpoints, but "r breakpoints" only works if we're at a breakpoint.
*/
void MainWindow::debug_delete_go()
{
  mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "SIGNAL SQLSTATE '05678' SET message_text='This statement is not supported at this time'");
  put_diagnostics_in_result();
}


/*
  We will get here if mouse button pressed on debug_tab_widget (we don't know which debug_widget yet, possibly none of them).
  What's it good for? Probably this will be the main way to set a breakpoint.
  I think we'll need to find a right mousebutton press and put up a menu.
  But maybe it would be better if debug_window had a menu bar.
  In ddd, one of the many options is: double-click. But I didn't succeed with doubleclick.
  Todo: figure out what to do if there's already a breakpoint here. Clear it? Right now we're just duplicating it.
*/
void MainWindow::action_debug_mousebuttonpress(QEvent *event, int which_debug_widget_index)
{
  int line_number;
  char command[512];
  char schema_name[512];
  char routine_name[512];
  int debug_widget_index;

  QMouseEvent *mouse_event= static_cast<QMouseEvent*> (event);            /* Get line number where mousepress happened */
  QPoint point= mouse_event->pos();
  debug_widget_index= debug_tab_widget->currentIndex();
  /* I saw a crash once so maybe these checks aren't to paranoid. But I don't know whether they avoid the crash. */
  if (debug_widget_index == -1) return;                                   /* Check for an impossible condition */
  if (debug_widget_index > DEBUG_TAB_WIDGET_MAX) return;                  /* "" */
  if (which_debug_widget_index >= debug_tab_widget->count()) return;      /* "" */
  if (which_debug_widget_index != debug_widget_index) return;             /* "" */
  QTextCursor text_cursor= debug_widget[debug_widget_index]->cursorForPosition(point);
  line_number= text_cursor.blockNumber() + 1;

  strcpy(schema_name, debug_routine_schema_name[debug_widget_index].toUtf8());
  strcpy(routine_name, debug_routine_name[debug_widget_index].toUtf8());

  sprintf(command, "$breakpoint %s.%s %d", schema_name, routine_name, line_number);

  statement_edit_widget->setPlainText(command);                           /* Execute "$breakpoint ..." */
  action_execute();
}


/*
  Debug|Clear
  Similar code is in action_debug_breakpoint().
  Do not confuse with Debug|Delete: "$delete 5" clears a breakpoint #5.
  Todo: Check that somewhere something says debug_maintain_prompt(2, debug_widget_index, line_number);
*/
void MainWindow::action_debug_clear()
{
  char command[512];
  int line_number;
  char line_number_as_string[10];

  int debug_widget_index= debug_tab_widget->currentIndex();
  /* I can't imagine how currentIndex() could be == -1, but if it is, do nothing. */
  if (debug_widget_index < 0) return;
  line_number= debug_widget[debug_widget_index]->block_number + 1;
  sprintf(line_number_as_string, "%d", line_number);
  strcpy(command, "$CLEAR ");
  strcat(command, debug_routine_schema_name[debug_widget_index].toUtf8());
  strcat(command,".");
  strcat(command, debug_routine_name[debug_widget_index].toUtf8());
  strcat(command, " ");
  strcat(command, line_number_as_string);

  statement_edit_widget->setPlainText(command);
  action_execute();
}


/*
  Debug|Continue -- "CALL xxxmdbug.command([channel-name],'continue');" as a client statement
*/
void MainWindow::action_debug_continue()
{
  statement_edit_widget->setPlainText("$CONTINUE");
  action_execute();
}

/*
  Debug|Step
  This is much like Debug|Continue, but we don't care if we end up on a permanent breakpoint.
*/
void MainWindow::action_debug_step()
{
  statement_edit_widget->setPlainText("$STEP");
  action_execute();
}

/*
  Debug|Leave
*/
//void MainWindow::action_debug_leave()
//{
//    statement_edit_widget->setPlainText("$LEAVE");
//    action_execute();
//}


/* $SKIP seems to act like $CONT which isn't terribly useful */
void MainWindow::debug_skip_go()
{
  mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "SIGNAL SQLSTATE '05678' SET message_text='The $SKIP statement is not supported at this time'");
  put_diagnostics_in_result();
}


void MainWindow::debug_source_go()
{
  mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "SIGNAL SQLSTATE '05678' SET message_text='The $SOURCE statement is not supported at this time'");
  put_diagnostics_in_result();
}


/*
  $SET declared_variable_name = value;
  Todo: As an additional error check: look up declared_variable_name in xxxmdbug.variables.
*/
void MainWindow::debug_set_go(QString text)
{
  char command_string[5120];
  int index_of_number_1, index_of_number_2;

  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }
  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    if (debug_error((char*)"Overflow") != 0) return;
    return;
  }
  debug_call_xxxmdbug_command(command_string);
  put_diagnostics_in_result();
}


/*
  $execute sql-statement
  todo: this will fail if first token is a comment
  todo: get rid of this, it fails
*/
void MainWindow::debug_execute_go()
{
  mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "SIGNAL SQLSTATE '05678' SET message_text='The $EXECUTE statement is not supported at this time'");
  put_diagnostics_in_result();

//  QString s;
//  char command_string[5120];
//  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
//  {
//    if (debug_error((char*)"No debug session in progress") != 0) return;
//  }
//  if (debug_error((char*)"") != 0) return;
//
//  s= "execute ";
//  s.append(text.right(text.length() - (main_token_offsets[main_token_number] + main_token_lengths[main_token_number])));
//  QMessageBox msgbox;
//  msgbox.setText(s);
//  msgbox.exec();
//  strcpy(command_string, s.toUtf8());
//  printf("command_string=%s.\n", command_string);
//  debug_call_xxxmdbug_command(s.toUtf8());
//  put_diagnostics_in_result();
////  mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "SIGNAL SQLSTATE '05678' SET message_text='This statement is not supported at this time'");
////  put_diagnostics_in_result();
}


/* For: $next, $continue, $refresh */
/* The following could be used for all the $... statements where one merely passes the command on to the debuggee */
/* We strip the comments and the ; but if there's junk after the first word it will cause an error, as it should. */
/* Todo: Should you add a semicolon? */
void MainWindow::debug_other_go(QString text)
{
  char command_string[512];
  int index_of_number_1, index_of_number_2;
  QString q_schema_name, q_routine_name;

  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }
  if (debug_parse_statement(text, command_string, &index_of_number_1, &index_of_number_2) < 0)
  {
    if (debug_error((char*)"Overflow") != 0) return;
    return;
  }
  q_schema_name= debug_q_schema_name_in_statement;
  q_routine_name= debug_q_routine_name_in_statement;

  if (debug_error((char*)"") != 0) return;
  if (debug_call_xxxmdbug_command(command_string) != 0) return;
  put_diagnostics_in_result();
}


/*
  Debug|Next
*/
void MainWindow::action_debug_next()
{
 statement_edit_widget->setPlainText("$NEXT");
 action_execute();
}


/*
  Debug|Skip
  Commented out -- not working at this time
*/
//void MainWindow::action_debug_skip()
//{
//  statement_edit_widget->setPlainText("$SKIP");
//  action_execute();
//}


/*
  Given 'information status' schema + routine name, find out what the tab number is and make it current.
  Then change the cursor to point to a particular line.
  After the cursor is set to the line, the CodeEditor's highlightCurrentLine wil highlight it.
  Remember, blockNumber() is base 0
  Todo: If the call was completed, you shouldn't be here -- the line number will be too big!
  Todo: Make sure highlight line goes off if execution switches to a different routine's tab. (I think that's okay now.)
*/
void MainWindow::debug_highlight_line()
{
  QTextCursor cursor;
  int debug_widget_index;
  int new_line_number;
  QString debuggee_schema_identifier;
  QString debuggee_routine_identifier;

  for (debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    debuggee_schema_identifier= debuggee_information_status_schema_identifier;
    debuggee_schema_identifier= connect_stripper(debuggee_schema_identifier);
    debuggee_routine_identifier= debuggee_information_status_routine_identifier;
    debuggee_routine_identifier= connect_stripper(debuggee_routine_identifier);
    if (QString::compare(debuggee_schema_identifier, debug_routine_schema_name[debug_widget_index]) == 0)
    {
      if (QString::compare(debuggee_routine_identifier, debug_routine_name[debug_widget_index]) == 0)
      {
        break; /* now we know that debug_widget_index is for the tab of the routine the debuggee's at */
      }
    }
  }

  if (debug_widget_index >= DEBUG_TAB_WIDGET_MAX)
  {
    /* I think it's impossible that routine won't be there, but if it isn't, do nothing. */
    /* Well, not quite impossible -- schema or routine might be "unknown" or "". That's probably harmless. */
    /* Todo: Change status line, on supposition that the routine has not started or has ended. */
    return;
  }

  debug_tab_widget->setCurrentWidget(debug_widget[debug_widget_index]);

  new_line_number= atoi(debuggee_information_status_line_number);

  --new_line_number;

  /*
    Something like QTextCursor::HighlightCurrentLine. Make the background light red.
    This cancels the "current line" background (which is light yellow), and can be cancelled by it -- I think that's okay.
  */

  /* If this is a different debug_widget than the last one, this should turn highlight off for the last one. */
  if (debug_widget_index != debug_timer_old_debug_widget_index)
  {
    if (debug_timer_old_debug_widget_index != -1)
    {
      if (debug_timer_old_debug_widget_index != -1)
      {
        QList<QTextEdit::ExtraSelection> old_extraSelections;
        debug_widget[debug_timer_old_debug_widget_index]->setExtraSelections(old_extraSelections);
      }
    }
    debug_timer_old_debug_widget_index= debug_widget_index;
  }

  QList<QTextEdit::ExtraSelection> extraSelections;

  QTextEdit::ExtraSelection selection;

  QColor lineColor= QColor(Qt::red).lighter(160);

  QTextDocument* doc= debug_widget[debug_widget_index]->document();
  QTextBlock block;
  /* new_line_number will be -2 if "debuggee_wait_loop ended". probably -1 is impossible */
  if (new_line_number == -2) block= doc->findBlockByNumber(doc->blockCount() - 1);
  else if (new_line_number < 0) block= doc->findBlockByNumber(0);
  else block= doc->findBlockByNumber(new_line_number);
  int pos= block.position();

  selection.format.setBackground(lineColor);
  selection.format.setProperty(QTextFormat::FullWidthSelection, true);
  selection.cursor= QTextCursor(doc);
  selection.cursor.setPosition(pos, QTextCursor::MoveAnchor);
  selection.cursor.setPosition(pos+1, QTextCursor::KeepAnchor);
  selection.cursor.clearSelection();
  extraSelections.append(selection);
  debug_widget[debug_widget_index]->setExtraSelections(extraSelections);
}


/*
  Debug|Delete
*/
//void MainWindow::action_debug_delete()
//{
//  printf("STUB: delete\n");
//}


/*
  Debug|Exit
  = "CALL xxxmdbug.command([channel-name],'exit');" as a client statement
  Check debuggee_state. If it's not DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP, debuggee_wait_loop() is not going on. THIS APPLIES FOR MANY COMMANDS!
  Send "exit" command, which should cause debuggee to exit debuggee_wait_loop() and return from the thread function.
  Wait up to 1 second for "exit" to work.
  If it didn't work, "kill debuggee-connection-id".
  TODO: If debuggee fails to respond, it's up to you to kill it.
  TODO: Call this when main window exits, otherwise the thread's connection is immortal.
  TODO: I wish there was some way to clear up init_connect now too.
  Todo: menu item to shut down debugger, and hide debug_tab_widget, close the connection, and kill the thread.
  Todo: when would you like to get rid of debug_widgets[]?
  We call debug_menu_enable_or_disable(TOKEN_KEYWORD_DEBUG_EXIT) after calling debug_exit_go().
*/
void MainWindow::action_debug_exit()
{
    statement_edit_widget->setPlainText("$EXIT");
    action_execute();
}


/* todo: somewhere there should be a pthread_join with the thread_id made in debug_debug_go() */
/* flagger == 0 means this is a regular $exit; flagger == 1 means we're getting rid of stuff due to a severe severe error */
void MainWindow::debug_exit_go(int flagger)
{
  char call_statement[512];

  if (flagger == 0)
  {
    /* Todo: merge this with debug_error somehow, and make sure nothing's enabled/disabled unless debug/exit succeeded! */
    if (menu_debug_action_exit->isEnabled() == false)
    {
      mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "signal sqlstate '05678' set message_text = '$DEBUG not done'");
      put_diagnostics_in_result();
      return;
    }
  }

  debug_top_widget->hide();
  debug_timer->stop();
  /* Todo: more cleanup e.g. kill the debuggee thread, close the connection, even if it's not responding */
  /* Todo: yet more cleanup if you want to get rid of the setup result */

  debug_delete_tab_widgets();

  if (debuggee_state == DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_call_xxxmdbug_command("exit") != 0)
    {
      put_diagnostics_in_result();
      return;
    }
    for (int kk= 0; kk < 10; ++kk) {QThread48::msleep(100); if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP) break; }
    if ((debuggee_state == DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP) && (debuggee_connection_id != 0))
    {
      sprintf(call_statement, "kill %d", debuggee_connection_id);

      if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
      {
        sprintf(call_statement, "SIGNAL SQLSTATE '05678' SET message_text='exit failed and kill failed -- try executing manually: kill %d'", debuggee_connection_id);
        mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement));
      }
    }
  }

  /* This should erase left-over messages in the pipe, such as the 'i status' message. Probably there's only one. */
  QString error_return;
  sprintf(call_statement, "%s%s%s","call xxxmdbug.dbms_pipe_receive('xxxmdbug_",debuggee_channel_name,
          "', 1, @xxxmdbug_tmp_1, @xxxmdbug_tmp_2)");
  for (int i= 0; i < 50; ++i)                                              /* 50 is an arbitrary big number */
  {
    if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement))) break;
    error_return= select_1_row("select @xxxmdbug_tmp_1");
    if (error_return != "") break;
    if (select_1_row_result_1 == "") break;
  }

  /* $install was saying "Suggested next step is: $EXIT" even though $EXIT was already done. */
  debuggee_state= DEBUGGEE_STATE_0;

  if (flagger == 0)
  {
    put_diagnostics_in_result(); /* This should show the result of the final call or select, so it should be "ok" */
  }
}


/* This is called from $exit, but also might be called from $debug if $debug fails */
/* We're reversing the effects of "new CodeEditor() and "addTab()" in debug_debug_go(). */
void MainWindow::debug_delete_tab_widgets()
{
  debug_tab_widget->clear(); /* equivalent to removeTab() for all tab widgets */
  for (int debug_widget_index= 0; debug_widget_index < DEBUG_TAB_WIDGET_MAX; ++debug_widget_index)
  {
    if (debug_widget[debug_widget_index] != 0)
    {
      debug_widget[debug_widget_index]->removeEventFilter(this);
      delete debug_widget[debug_widget_index];
      debug_widget[debug_widget_index]= 0;
    }
  }
}


/*
  Debug|Information
*/
void MainWindow::action_debug_information()
{
  if (debug_call_xxxmdbug_command("information status") != 0) return;
}


/*
  Debug|Refresh server variables
*/
void MainWindow::action_debug_refresh_server_variables()
{
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }
  if (debug_call_xxxmdbug_command("refresh server variables") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.server_variables");
  emit action_execute();
}


/*
  Debug|Refresh user variables
*/
void MainWindow::action_debug_refresh_user_variables()
{
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }
  if (debug_call_xxxmdbug_command("refresh user variables") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.user_variables");
  emit action_execute();
}


/*
  Debug|Refresh variables
*/
void MainWindow::action_debug_refresh_variables()
{
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }
  if (debug_call_xxxmdbug_command("refresh variables") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.variables");
  emit action_execute();
}


/*
  Debug|Refresh call_stack
*/
void MainWindow::action_debug_refresh_call_stack()
{
  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debug_error((char*)"No debug session in progress") != 0) return;
  }
  if (debug_call_xxxmdbug_command("refresh call_stack") != 0) return;
  statement_edit_widget->insertPlainText("select * from xxxmdbug.call_stack");
  emit action_execute();
}


/*
  Called from: action_debug_exit(), action_debug_debug(), action_debug_information(),
  and anything else which requires xxxmdbug.command -- except 'attach', except
  'information status' from from action_debug_timer_status().
  Make the SQL statement that is actually executed, but don't trouble the user with
  the details by showing it. For example, if the user said "$DEBUG test.p8", we
  execute "call xxxmdbug.command('channel#...','debug test.p8')" and return the
  result from that.
*/
int MainWindow::debug_call_xxxmdbug_command(const char *command)
{
  char call_statement[512];

  strcpy(call_statement, "call xxxmdbug.command('");
  strcat(call_statement, debuggee_channel_name);
  strcat(call_statement, "', '");
  strcat(call_statement, command);
  strcat(call_statement, "');\n");

  if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
  {
    /* Initially this can happen because we start the timer immediately after we call 'attach'. */
    put_diagnostics_in_result();
    return 1;
  }
  debug_statement= command;

  //put_diagnostics_in_result()???

  /* We no longer try to get the debuggee response with debug_information_status(). */
  //if (strcmp(command, "information status") != 0)
  //{
  //  QString debuggee_response= "Debuggee response = ";
  //  if (debug_information_status((char *)command) != 0)
  //    {
  //    debuggee_response.append("(none). Possibly debuggee is still executing the last command, which was: ");
  //    debuggee_response.append(debuggee_information_status_last_command);
  //  }
  //  else
  //  {
  //    debuggee_response.append(debuggee_information_status_last_command_result);
  //  }
  //  history_edit_widget->append(debuggee_response);
  //}
  return 0;
}


/*
  INFORMATION STATUS -- internal
  We should call this after 'attach' or after 'debug' to see whether there's been some success.
  Most contacts with the debugger should get a quick response, unless the last was 'continue' or 'step ...' (?)
  ... Actually, check that: maybe 'continue' acknowledges receipt before continuing.
  ... And I think that 'continue' will still cause messages, if it's inside stored-procedure code that's being debugged.
*/
int MainWindow::debug_information_status(char *last_command)
{
  int k;

  /* TODO: JUST GET RID OF THIS PROC. I NO LONGER SEE ANY POINT TO IT. */

  for (k= 0; k < 100; ++k)
  {
    /*
       By sleeping, we give the QTimer a chance to call action_debug_timer_status() and update status.
       But it doesn't work! I guess it's because sleeping doesn't restart the event loop!
    */
    QThread48::msleep(100);
    if (strcmp(debuggee_information_status_last_command, last_command) == 0)
    {
      /* todo: this is not enough! you should be checking also if the command number has gone up! */
      break;
    }
  }

  /* k >= 100 means the 100-iteration loop containing a 10-ms sleep has gone by without a match for last_commnd */
  /* the caller will probably generate a warning */
  if (k >= 100) return 1; else return 0;
}


/*
  QTimer *debug_timer: every 100 milliseconds (every 1/10 second), while the debuggee
  is running, the timer event will invoke action_debug_timer() which updates the status.
  Todo: Start it when debuggee starts, stop it when debuggee stops.
  If a new information status message has appeared, display it.
  Todo: make sure timer is stopped before debug_tab_window is closed.
  Todo: This won't necessarily catch errors, because of the 1/10 second intervals and
        because multiple commands can be sent asynchronously. Think of a way!
        Meanwhile, just say: "If you must know the result, you must wait after each command."
  Todo: It would be faster to interpret the 'information status' results in C code here,
        rather than using xxxmdbug.information_status repeatedly.
        Besides, selecting from xxxmdbug.information_status can change the state of the main mysql connection.
        Maybe you should create a different connection?
  Todo: Worry about race conditions. The main thread could look at a status variable before all
        the status variables are updaated by this function. This might be alleviated by
        updating the counter last, but some sort of mutex might have to be considered.
  Todo: Check whether something closes the connection.
        Maybe it would be safest to say that, once a connection is open, it stays open.
        Hmm, why is this a worry, since we're doing the searching on main connection not debuggee connection?
  Todo: I have seen the xxxmdbug.command() cause "send+receive error" meaning a change to init_connect failed.
        Maybe xxxmdbug.commnd() shouldn't try to change init_connect -- after all, we're not trying to send, only receive.

*/
void MainWindow::action_debug_timer_status()
{
  char call_statement[512];
  MYSQL_RES *debug_res= NULL;
  MYSQL_ROW row= NULL;
  char unexpected_error[512];
  int unexpected_new_error= 0;

  /*
    DANGER
    When the main line is running SQL statements, it calls
    QApplication::processEvents(), so there can be horrible
    conflicts -- action_debug_timer_status() runs SQL too.
    Either we must disable the Run|Kill feature, or disable
    the timer. Here, we've chosen to disable the timer.
  */
  if (dbms_long_query_state == LONG_QUERY_STATE_STARTED) return;

  unexpected_error[0]= '\0';
  strcpy(call_statement, "call xxxmdbug.command('");
  strcat(call_statement, debuggee_channel_name);
  strcat(call_statement, "', 'information status')");

  if (debuggee_state != DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP)
  {
    if (debuggee_state == DEBUGGEE_STATE_DEBUGGEE_WAIT_LOOP_ERROR)
    {
      strcpy(unexpected_error, debuggee_state_error);
      strcat(unexpected_error, ". maybe a new $SETUP needed? cannot continue. Suggested next step is: $EXIT");
    }
    else
    {
      if ((debuggee_state < 0) || (debuggee_state == DEBUGGEE_STATE_END)) strcpy(unexpected_error, "debuggee_wait_loop ended");
      else strcpy(unexpected_error, "debuggee_wait_loop() is not happening");
    }
  }

  if (unexpected_error[0] == '\0')
  {
    if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], call_statement, strlen(call_statement)))
    {
      /* Initially this can happen because we start the timer immediately after we call 'attach'. */
      strcpy(unexpected_error, "i status command failed");
    }
  }

  if (unexpected_error[0]== '\0')
  {
    const char *query= "select * from xxxmdbug.information_status";
    if (mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], query, strlen(query)))
    {
      strcpy(unexpected_error, "i status command failed (this is not always a severe error)");
    }
  }

  if (unexpected_error[0] == '\0')
  {
    debug_res= mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
    if (debug_res == NULL)
    {
      strcpy(unexpected_error, "mysql_store_result failed");
    }
  }

  if (unexpected_error[0] == '\0')
  {
    row= mysql_fetch_row(debug_res);
    if (row == NULL)
    {
      strcpy(unexpected_error, "mysql_fetch row failed");
    }
    else
    {
      if (mysql_num_fields(debug_res) < 14)
      {
        strcpy(unexpected_error, "mysql_num_fields < 14");
      }
      else
      {
        /* Just kludging -- I think something is overflowing. Todo: Find out what's really going on. */
        memset(debuggee_information_status_debugger_name, 0, 32);
        memset(debuggee_information_status_debugger_version, 0, 8);
        memset(debuggee_information_status_timestamp_of_status_message, 0, 32);
        memset(debuggee_information_status_number_of_status_message, 0, 8);
        memset(debuggee_information_status_icc_count, 0, 8);
        memset(debuggee_information_status_schema_identifier, 0, 256);
        memset(debuggee_information_status_routine_identifier, 0, 256);
        memset(debuggee_information_status_line_number, 0, 8);
        memset(debuggee_information_status_is_at_breakpoint, 0, 8);
        memset(debuggee_information_status_is_at_tbreakpoint, 0, 8);
        memset(debuggee_information_status_is_at_routine_exit, 0, 8);
        memset(debuggee_information_status_stack_depth, 0, 8);
        memset(debuggee_information_status_last_command, 0, 256);
        memset(debuggee_information_status_last_command_result, 0, 256);
        memset(debuggee_information_status_commands_count, 0, 8);

        if (row[0] != NULL) strncpy(debuggee_information_status_debugger_name, row[0], 32 - 1);
        if (row[1] != NULL) strncpy(debuggee_information_status_debugger_version, row[1], 8 - 1);
        if (row[2] != NULL) strncpy(debuggee_information_status_timestamp_of_status_message, row[2], 32 - 1);
        if (row[3] != NULL) strncpy(debuggee_information_status_number_of_status_message, row[3], 8 - 1);
        if (row[4] != NULL) strncpy(debuggee_information_status_icc_count, row[4], 8 - 1);
        if (row[5] != NULL) strncpy(debuggee_information_status_schema_identifier, row[5], 256 - 1);
        if (row[6] != NULL) strncpy(debuggee_information_status_routine_identifier, row[6], 256 - 1);
        if (row[7] != NULL) strncpy(debuggee_information_status_line_number, row[7], 8 - 1);
        if (row[8] != NULL) strncpy(debuggee_information_status_is_at_breakpoint, row[8], 8 - 1);
        if (row[9] != NULL) strncpy(debuggee_information_status_is_at_tbreakpoint, row[9], 8 - 1);
        if (row[10] != NULL) strncpy(debuggee_information_status_is_at_routine_exit, row[10], 8 - 1);
        if (row[11] != NULL) strncpy(debuggee_information_status_stack_depth, row[11], 8 - 1);
        if (row[12] != NULL) strncpy(debuggee_information_status_last_command, row[12], 256 - 1);
        if (row[13] != NULL) strncpy(debuggee_information_status_last_command_result, row[13], 256 - 1);
        if (row[14] != NULL) strncpy(debuggee_information_status_commands_count, row[14], 8 - 1);
      }
    }
  }

  if (debug_res != NULL) mysql_free_result(debug_res);

  if (unexpected_error[0] != '\0')
  {
    if (strcmp(debuggee_information_status_last_command_result, unexpected_error) != 0)
    {
      /* This should be considered as a status change, as if it's a new i status message */
      unexpected_new_error= 1;
      strcpy(debuggee_information_status_last_command_result, unexpected_error);
    }
  }

  //  printf("debuggee_information_status_debugger_name=%s.\n", debuggee_information_status_debugger_name);
  //  printf("debuggee_information_status_debugger_version=%s.\n", debuggee_information_status_debugger_version);
  //  printf("debuggee_information_status_timestamp_of_status_message=%s.\n", debuggee_information_status_timestamp_of_status_message);
  //  printf("debuggee_information_status_number_of_status_message=%s.\n", debuggee_information_status_number_of_status_message);
  //  printf("debuggee_information_status_icc_count=%s.\n", debuggee_information_status_icc_count);
  //  printf("debuggee_information_status_schema_identifier=%s.\n", debuggee_information_status_schema_identifier);
  //  printf("debuggee_information_status_routine_identifier=%s.\n", debuggee_information_status_routine_identifier);
  //  printf("debuggee_information_status_line_number=%s.\n", debuggee_information_status_line_number);
  //  printf("debuggee_information_status_is_at_breakpoint=%s.\n", debuggee_information_status_is_at_breakpoint);
  //  printf("debuggee_information_status_is_at_tbreakpoint=%s.\n", debuggee_information_status_is_at_tbreakpoint);
  //  printf("debuggee_information_status_is_at_routine_exit=%s.\n", debuggee_information_status_is_at_routine_exit);
  //  printf("debuggee_information_status_stack_depth=%s.\n", debuggee_information_status_stack_depth);
  //  printf("debuggee_information_status_last_command=%s.\n", debuggee_information_status_last_command);
  //  printf("debuggee_information_status_last_command_result=%s.\n", debuggee_information_status_last_command_result);
  //  printf("debuggee_information_status_commands_count=%s.\n", debuggee_information_status_commands_count);

  /* If a status message would probably confuse a rational user, change it. */
    if (strstr(debuggee_information_status_last_command_result, "completed CALL") != NULL)
      strcpy(debuggee_information_status_last_command_result, "Routine has stopped. Suggested next step is: $EXIT");

    if (strstr(debuggee_information_status_last_command_result, "Failed, the expected command is debug") != NULL)
      strcpy(debuggee_information_status_last_command_result, "Routine has stopped and continuing past the end is not possible.  Suggested next step is: $EXIT");

  /*
    When execution ends, we still have line number = 0 and "stopped at breakpoint".
    That's misleading, but rather than change MDBug, let's override what it returns.
    Also: last command isn't always updated if execution has ended.
  */
  if (strcmp(debuggee_information_status_last_command_result, "debuggee_wait_loop ended") == 0)
  {
    strcpy(debuggee_information_status_line_number, "-1");
    strcpy(debuggee_information_status_is_at_breakpoint, "no");
    strcpy(debuggee_information_status_is_at_routine_exit, "yes");
    strcpy(debuggee_information_status_last_command, debug_statement.toUtf8());
    /* todo: what about debuggee_information_status_last_command_result? */
  }

  /*
    Change line_edit i.e. status widget, and maybe change highlight, if there's a new message.
    It would be extremely wasteful to update every time, i.e. every 1/10 second, if there's no change.
    Apparently the change that you can depend on is icc_count, not commands_count or number_of_status_message.
    I don't think there's any need to check whether line number + routine name have changed, but see below.
  */
  if ((debug_timer_old_commands_count != atoi(debuggee_information_status_commands_count))
  ||  (debug_timer_old_number_of_status_message != atoi(debuggee_information_status_last_command))
  ||  (debug_timer_old_icc_count != atoi(debuggee_information_status_icc_count))
  ||  (debug_timer_old_line_number != atoi(debuggee_information_status_line_number))
  ||  (unexpected_new_error == 1))
  {
    char result[2048];

    strcpy(result, "Debugger status = ");
    if (strcmp(debuggee_information_status_schema_identifier, "unknown") != 0)
    {
      strcat(result, "(Current position: ");
      strcat(result, debuggee_information_status_schema_identifier);
      strcat(result, ".");
      strcat(result, debuggee_information_status_routine_identifier);
      strcat(result, " line ");
      strcat(result, debuggee_information_status_line_number);
      strcat(result, ")");
    }
    strcat(result, "(Last debug command: ");
    strcat(result, debuggee_information_status_last_command);
    strcat(result, ", result: ");
    strcat(result, debuggee_information_status_last_command_result);
    strcat(result, ")");

    if (strcmp(debuggee_information_status_is_at_breakpoint, "yes") == 0)
    {
      strcat(result, "(STOPPED AT BREAKPOINT)");
    }

    if (strcmp(debuggee_information_status_is_at_routine_exit, "yes") == 0)
    {
      strcat(result, "(STOPPED AT ROUTINE EXIT)");
    }

    debug_line_widget->setText(result);

    /*
      If and only if the line number or routine name or schema name has changed,
      highlight i.e. change background color in order to show where we're at now.
      Formerly this checked "if (strcmp(debuggee_information_status_is_at_breakpoint, "yes") == 0)"
      i.e. are we at a breakpoint, but I no longer see why that matters.
      Besides, '$breakpoint' does not move the position.
    */
    if ((debug_timer_old_line_number != atoi(debuggee_information_status_line_number))
     || (strcmp(debug_timer_old_schema_identifier, debuggee_information_status_schema_identifier) == 0)
     || (strcmp(debug_timer_old_routine_identifier, debuggee_information_status_routine_identifier) == 0))
    {
      debug_highlight_line();
      debug_timer_old_line_number= atoi(debuggee_information_status_line_number);
      strcpy(debug_timer_old_schema_identifier, debuggee_information_status_schema_identifier);
      strcpy(debug_timer_old_routine_identifier, debuggee_information_status_routine_identifier);
    }
    debug_timer_old_commands_count= atoi(debuggee_information_status_commands_count);
    debug_timer_old_number_of_status_message= atoi(debuggee_information_status_last_command);
    debug_timer_old_icc_count= atoi(debuggee_information_status_icc_count);
  }
}
#endif


/*
  For menu item "Execute|Kill" we must start another thread,
  which connects and issues a Kill query-id.
  The kill thread has a subset of the actions of the debug thread so see also debug_thread.
  Possible variant: keep it alive and use it for monitoring.
*/
void* kill_thread(void* unused)
{
  (void) unused; /* suppress "unused parameter" warning */
  char call_statement[512];
  int is_connected;

  is_connected= 0;
  kill_state= KILL_STATE_CONNECT_THREAD_STARTED;
  for (;;)
  {
    if (options_and_connect(MYSQL_KILL_CONNECTION))
    {
      kill_state= KILL_STATE_CONNECT_FAILED;
      break;
    }
    is_connected= 1;
    kill_state= KILL_STATE_IS_CONNECTED;
    sprintf(call_statement, "kill query %d", kill_connection_id);
    if (mysql_real_query(&mysql[MYSQL_KILL_CONNECTION], call_statement, strlen(call_statement)))
    {
      kill_state= KILL_STATE_MYSQL_REAL_QUERY_ERROR;
      break;
    }
    kill_state= KILL_STATE_ENDED;
    break;
  }
  if (is_connected == 1) mysql_close(&mysql[MYSQL_DEBUGGER_CONNECTION]);
  return ((void*) NULL);
}

bool is_kill_requested;

void MainWindow::action_kill()
{
  pthread_t thread_id;

  if (is_mysql_connected != 1) return; /* impossible */
  is_kill_requested= true;
  if (dbms_long_query_state == LONG_QUERY_STATE_STARTED)
  {
    kill_connection_id= statement_edit_widget->dbms_connection_id;
    pthread_create(&thread_id, NULL, &kill_thread, NULL);
    /* no pthread_join here -- what would be the point of waiting? */
  }
}

/*
  Implementing ^C i.e. control-C i.e. menu item Run|Kill ...
  Once an SQL statement or SQL statement series has been accepted, user input
  is restricted to "kill" (a few other events are tolerated but not ones that cause statements),
  so around the loop inside action_execute() there is some enabling and disabling.
  SQL statements are done in a separate thread, dbms_long_query();
  in the main thread there is a wait-loop which stops when the thread ends;
  the wait-loop includes QApplication::ProcessEvents() so a "Kill" will be seen.
  "Kill" causes the SQL statement to end (if there's one running).
  ^C is an appropriate shortcut although it usually means "clip".
  Kill sets a flag so that if there are multiple statements, they are all aborted.
  dbms_long_query is only for potentially-slow SQL statements that might need to be killed.
  Todo: this could be done via a permanently-existing thread or pool of threads.
  Todo: QThread is more portable than pthread, although it looks harder to understand.
  Todo: put this together with the dbms_* routines in ocelotgui.h in a separate class.
*/
void* dbms_long_query_thread(void* unused)
{
 (void) unused; /* suppress "unused parameter" warning */

  dbms_long_query_result= mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], dbms_query, dbms_query_len);
  dbms_long_query_state= LONG_QUERY_STATE_ENDED;
  return ((void*) NULL);
}

void* dbms_long_next_result_thread(void* unused)
{
  (void) unused; /* suppress "unused parameter" warning */

  dbms_long_query_result= mysql_next_result(&mysql[MYSQL_MAIN_CONNECTION]);
  dbms_long_query_state= LONG_QUERY_STATE_ENDED;
  return ((void*) NULL);
}


/*
  For menu item "execute" we said (...SLOT(action_execute())));
  By default this is on and associated with File|Execute menu item.
  Execute what's in the statement widget.
  The statement widget might contain multiple statements.
*/
void MainWindow::action_execute()
{
  QString text;
  int returned_begin_count;

  main_token_number= 0;
  text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */

  /* While executing, we allow no more statements, but a few things are enabled. */
  menu_file->setEnabled(false);
  menu_edit->setEnabled(false);
  menu_run_action_execute->setEnabled(false);
  if (ocelot_sigint_ignore == 0) menu_run_action_kill->setEnabled(true);
  menu_settings->setEnabled(false);
  menu_debug->setEnabled(false);
  menu_help->setEnabled(false);
  statement_edit_widget->setReadOnly(true);
  is_kill_requested= false;
  for (;;)
  {
    main_token_count_in_statement= get_next_statement_in_string(main_token_number, &returned_begin_count);
    if (main_token_count_in_statement == 0) break;
    action_execute_one_statement(text);

    main_token_number+= main_token_count_in_statement;
    if (is_kill_requested == true) break;
  }

  menu_file->setEnabled(true);
  menu_edit->setEnabled(true);
  menu_run_action_execute->setEnabled(true);
  menu_run_action_kill->setEnabled(false);
  menu_settings->setEnabled(true);
  menu_debug->setEnabled(true);
  menu_help->setEnabled(true);
  statement_edit_widget->setReadOnly(false);

  statement_edit_widget->clear(); /* ?? this is supposed to be a slot. does that matter? */
  widget_sizer();

  /* Try to set history cursor at end so last line is visible. Todo: Make sure this is the right time to do it. */
  history_edit_widget->verticalScrollBar()->setValue(history_edit_widget->verticalScrollBar()->maximum());

  history_edit_widget->show(); /* Todo: find out if this is really necessary */
}

/*
  Todo: There are a few things yet to be considered.
     I'm not 100% confident about the way I'm clearing the old results out, e.g. "delete result_form_text[...]".
     Would it be more efficient to remove only the ones between new column count and old column count?
       No, because data type can change, it might not be a text field the next time around.
     Should I be saying "delete later" somehow?
     Does memory leak?
*/
void MainWindow::action_execute_one_statement(QString text)
{
  //QString text;
  MYSQL_RES *mysql_res_for_new_result_set;
  unsigned short int is_vertical= ocelot_result_grid_vertical; /* true if --vertical or \G or ego */

  ++statement_edit_widget->statement_count;

  /* Clear the old result sets. */
  //unsigned int old_result_column_count= result_grid_table_widget->result_column_count;

  /*
    Todo: There should be an indication that something is being executed.
    Possibly, on the title bar, setPlainText(tr("Executing ...")
    Possibly there should be a status widget or a progress bar.
    Whatever it is, turn it on now and turn it off when statement is done.
  */

  /*
    When we get a new result set, we want to remove all the old widgets inside result_grid_form.
    It's probably easy, but I didn't know how to get the original layout as a QFormLayout.
    So I delete the old layout too, and make a new one.
    Also I'm unsure whether the "delete item" will get rid of memory leaking, or merely fail.
    Update: "delete item;" fails so I commented it out. Maybe removeItem is failing too, I didn't check.
    Something about "must be implemented in subclasses" is possibly a hint that I'm doing it wrong.
    ... Perhaps I should create MAX_COLUMNS widgets at the start, and never delete them.
    ... Update: Qt documentation talks about clear() in places, maybe relevantly.
  */

  /* Apparently there is no need for a call to tokenize() here, it seems certain that it's already been called. */

  int query_utf16_len= main_token_offsets[main_token_number+main_token_count_in_statement - 1]
                       + main_token_lengths[main_token_number+main_token_count_in_statement - 1]
                       - main_token_offsets[main_token_number];

  query_utf16= text.mid(main_token_offsets[main_token_number], query_utf16_len);

  QString last_token;
  bool strip_last_token= false;
  int length_of_last_token_in_statement= main_token_lengths[main_token_number+main_token_count_in_statement - 1];
  int type_of_last_token_in_statement= main_token_types[main_token_number+main_token_count_in_statement - 1];
  last_token= text.mid(main_token_offsets[main_token_number+main_token_count_in_statement - 1],
                       length_of_last_token_in_statement);
  if ((length_of_last_token_in_statement == 1) && ((type_of_last_token_in_statement == TOKEN_KEYWORD_EGO) || (type_of_last_token_in_statement == TOKEN_KEYWORD_GO)))
  {
    length_of_last_token_in_statement= 2;
    strip_last_token= true;
    if (type_of_last_token_in_statement == TOKEN_KEYWORD_EGO) is_vertical= 1;
  }

  /* Strip last word if it's delimiter or (when --named-commands, not only token) go|ego. */
  if (last_token == ocelot_delimiter_str) strip_last_token= true;
  else if ((ocelot_named_commands > 0) && (main_token_count_in_statement > 1))
  {
    if (type_of_last_token_in_statement == TOKEN_KEYWORD_GO) strip_last_token= true;
    if (type_of_last_token_in_statement == TOKEN_KEYWORD_EGO)
    {
      strip_last_token= true;
      is_vertical= 1;
    }
  }
  if (strip_last_token == true)
  {
    query_utf16_copy= text.mid(main_token_offsets[main_token_number], query_utf16_len-length_of_last_token_in_statement);
  }
  else query_utf16_copy= query_utf16;

  statement_edit_widget->start_time= QDateTime::currentMSecsSinceEpoch(); /* will be used for elapsed-time display */
  int ecs= execute_client_statement(text);
  if (ecs != 1)
  {
    /* The statement was not handled entirely by the client, it must be passed to the DBMS. */

    bool do_something= true;

    /* If DBMS is not (yet) connected, except for certain SET @ocelot_... statements, this is an error. */
    if (is_mysql_connected == 0)
    {
      if (ecs == 2) statement_edit_widget->result= tr("OK");
      else statement_edit_widget->result= tr("ERROR not connected");
      do_something= false;
    }
    /* If --one-database, and USE caused default database to change, error */
    if ((ocelot_one_database > 0) && (ocelot_database != statement_edit_widget->dbms_database))
    {
      statement_edit_widget->result= tr("ERROR due to --one-database");
      do_something= false;
    }
    if (do_something == true)
    {

      dbms_query_len= query_utf16_copy.toUtf8().size();           /* See comment "UTF8 Conversion" */
      dbms_query= new char[dbms_query_len + 1];
      dbms_query_len= make_statement_ready_to_send(text, query_utf16_copy,
                                                   dbms_query, dbms_query_len,
                                                   strip_last_token);
      pthread_t thread_id;
      dbms_long_query_state= LONG_QUERY_STATE_STARTED;
      pthread_create(&thread_id, NULL, &dbms_long_query_thread, NULL);
      for (;;)
      {
        QThread48::msleep(10);
        if (dbms_long_query_state == LONG_QUERY_STATE_ENDED) break;
        QApplication::processEvents();
      }
      pthread_join(thread_id, NULL);

 //     dbms_long_query_result= mysql_real_query(&mysql[MYSQL_MAIN_CONNECTION], dbms_query, dbms_query_len);
 //     dbms_long_query_state= LONG_QUERY_STATE_ENDED;


      if (dbms_long_query_result)
      {
        /* beep() hasn't been tested because getting sound to work on my computer is so hard */
        if (ocelot_no_beep == 0) QApplication::beep();
        delete []dbms_query;
      }
      else {
        delete []dbms_query;

        /*
          It was a successful SQL statement, and now look if it returned a result.
          If it did, as determined by looking at the mysql_res that mysql_store_result() returns,
          then free the previous mysql_res and delete the previous result grid,
          before setting up new ones.
          This means that statements which don't return result sets don't cause clearing
          of old result sets from the screen, e.g. SET has no effect on result grid.
          This means that momentarily there will be an old mysql_res and a new mysql_res.
          That takes up memory. If it were easy to check in advance whether a statement
          caused a result set (e.g. with mysql_next_result or by looking at whether the
          statement is SELECT SHOW etc.), that would be better.
          Todo: nothing is happening for multiple result sets, we throw away all but the first.
        */
        mysql_res_for_new_result_set= mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
        if (mysql_res_for_new_result_set == 0) {
          /*
            Last statement did not cause a result set. We could hide the grid and shrink the
            central window with "result_grid_table_widget[0]->hide()", but we don't.
          */
          ;
        }
        else
        {
          if (mysql_res != 0)
          {
            mysql_free_result(mysql_res); /* This is the place we free if myql_more_results wasn't true, see below. */
          }


          mysql_res= mysql_res_for_new_result_set;

          /* We need to think better what to do if we exceed MAX_COLUMNS */

          /*
            Todo: consider whether it would be appropriate to set grid width with
            result_grid_table_widget[0]->result_column_count= mysql_num_fields(mysql_res);
            but it may be unnecessary, and may cause a crash in garbage_collect()
          */

          result_row_count= mysql_num_rows(mysql_res);                /* this will be the height of the grid */

          for (int i_r= 0; i_r < RESULT_GRID_TAB_WIDGET_MAX; ++i_r)
          {
            result_grid_table_widget[i_r]->garbage_collect();
          }
          //QFont tmp_font;
          //tmp_font= result_grid_table_widget[0]->font();
          result_grid_table_widget[0]->fillup(mysql_res, this, mysql_more_results(&mysql[MYSQL_MAIN_CONNECTION]),
                                              is_vertical, ocelot_result_grid_column_names);
          result_grid_tab_widget->setCurrentWidget(result_grid_table_widget[0]);
          result_grid_tab_widget->tabBar()->hide();
          result_grid_table_widget[0]->show();
          result_grid_tab_widget->show(); /* Maybe this only has to happen once */

          /* Todo: small bug: elapsed_time calculation happens before mysql_next_result(). */
          /* You must call mysql_next_result() + mysql_free_result() if there are multiple sets */
          put_diagnostics_in_result(); /* Do this while we still have number of rows */
          history_markup_append();

          if (mysql_more_results(&mysql[MYSQL_MAIN_CONNECTION]))
          {
            mysql_free_result(mysql_res);
            /*
              We started with CLIENT_MULTI_RESULT flag (not CLIENT_MULTI_STATEMENT).
              We expect that a CALL to a stored procedure might return multiple result sets
              plus a status result at the end. The following lines try to pick up and display
              the extra result sets, up to a fixed maximum, and just throw away everything
              after that, to avoid the dreaded out-of-sync error message.
              If it's an ordinary select, mysql_free_result(mysql_res) happens later, see above.
            */
            int result_grid_table_widget_index= 1;
            for (;;)
            {

              dbms_long_query_state= LONG_QUERY_STATE_STARTED;
              pthread_create(&thread_id, NULL, &dbms_long_next_result_thread, NULL);

              for (;;)
              {
                QThread48::msleep(10);
                if (dbms_long_query_state == LONG_QUERY_STATE_ENDED) break;
                QApplication::processEvents();
              }
              pthread_join(thread_id, NULL);

              if (dbms_long_query_result != 0) break;

              mysql_res= mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);

              /* I think the following will help us avoid the "status" return. */
              if (mysql_res == NULL) continue;

              if (result_grid_table_widget_index < RESULT_GRID_TAB_WIDGET_MAX)
              {
                result_grid_tab_widget->tabBar()->show(); /* is this in the wrong place? */
                result_row_count= mysql_num_rows(mysql_res);                /* this will be the height of the grid */
                result_grid_table_widget[result_grid_table_widget_index]->fillup(mysql_res, this, true,
                                                                                 is_vertical, ocelot_result_grid_column_names);
                result_grid_table_widget[result_grid_table_widget_index]->show();
                ++result_grid_table_widget_index;
              }

              if (mysql_res != 0) mysql_free_result(mysql_res);

            }
            mysql_res= 0;
          }

          return;
        }
      }

      put_diagnostics_in_result();
    }
  }

  /* statement is over */

  history_markup_append(); /* add prompt+statement+result to history, with markup */

}


/*
 Handle "client statements" -- statements that the client itself executes.
  Possible client statements:
  SET @OCELOT_GRID_DETACHED= 1
  SET @OCELOT_GRID_DETACHED = 0 (todo)
  DELIMITER delimiter
  CONNECT ... (Todo: CONNECT depends entirely on settings, it should be possible to have arguments)
*/
/*
  Detach:
  Update: any code related to detaching is old and probably doesn't work.
  Would "floating" be a better word than "detached"? or "undock"? or "detachable"?
  ignore if already detached
  make new window etc. -- earlier the idea was dock widgets, but now I'm dubious
  append to "statements to pass on before execution of next real statement"
  return 1;  / * i.e. we don't pass on immediately, it's a client statement  * /

  Todo: Finishing off the "SET @OCELOT_GRID_DETACHED=1;" tricks ...
  None of the menu keys work. They must be slotted to the new window as well as the old window. E.g. control-Q.
  "SET @OCELOT_GRID_DETACHED= 0;" should reverse the effect.
  Consider the title bar. Should it be possible to close it, and what should the effect be?
  Initial window dimension + position should be = current widget dimention + position (unless it's hidden or obscured).
*/
/*
  Todo: We want all the mysql commands to work just the way that they work in mysql.
        We will not depend on TOKEN_KEYWORD_... for this because "\" and "q" are separate tokens,
        and "q| is case sensitive.
*/

int MainWindow::execute_client_statement(QString text)
{
//  int i= 0;
  int i2= 0;
  int  sub_token_offsets[10];
  int  sub_token_lengths[10];
  int  sub_token_types[10];
  int is_client_format= 0;
  int true_text_size, true_token_count; /* size and count ignoring delimiter */
  //QString text;                      /* Todo: see if text should be a global */
  unsigned int statement_type;

  statement_type= main_token_types[main_token_number];     /* We used to use main_statement_type */

  //text= statement_edit_widget->toPlainText(); /* or I could just pass this to tokenize() directly */

  /* Calculate size. Don't count delimiter unless it's a delimiter statement. */
  true_text_size= query_utf16_copy.size();

  int query_utf16_size= query_utf16.size();
  int query_utf16_copy_size= query_utf16_copy.size();
  if ((query_utf16_size == query_utf16_copy_size) || (statement_type == TOKEN_KEYWORD_DELIMITER))
  {
    /* there was no delimiter, or there was a delimiter but it's a delimiter statement */
    true_text_size= query_utf16_size;
    true_token_count= main_token_count_in_statement;
  }
  else
  {
    /* There was a delimiter, and it's not a delimiter statement */
    true_text_size= query_utf16_copy_size;
    true_token_count= main_token_count_in_statement - 1;
  }

  /* Make a copy of the first few tokens, ignoring comments. */
  for (unsigned int i= main_token_number; /* main_token_lengths[i] != 0 && */ i < main_token_number + true_token_count; ++i)
  {
    /* Todo: find out why you have to figure out type again -- isn't it already known? */
    QString s= text.mid(main_token_offsets[i], main_token_lengths[i]);
    int t= token_type(s.data(), main_token_lengths[i]);
    if (t == TOKEN_TYPE_COMMENT_WITH_SLASH
    ||  t == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE
    ||  t == TOKEN_TYPE_COMMENT_WITH_MINUS) continue;
    sub_token_offsets[i2]= main_token_offsets[i];
    sub_token_lengths[i2]= main_token_lengths[i];
    sub_token_types[i2]= main_token_types[i];
    ++i2;
    if (i2 > 8) break;
  }
  sub_token_offsets[i2]= 0;
  sub_token_lengths[i2]= 0;
  sub_token_types[i2]= 0;

  /*
    CONNECT or \r.
    Defaults should have been set earlier. The user had a chance to change the defaults with SET.
    Currently the only possible dbms is "mysql", otherwise nothing happens.
    Ignore any other words in the statement.
    Todo: accept the mysql-client syntax, which has a few extras on it for reconnect.
  */
  /* Todo: We could easily modify so that we don't need sub_token_..., we could just skip comments. */
  if (statement_type == TOKEN_KEYWORD_CONNECT)
  {
      if (QString::compare(ocelot_dbms, "mysql", Qt::CaseInsensitive) == 0) connect_mysql(MYSQL_MAIN_CONNECTION);
      return 1;
  }

  /* QUIT or \q. mysql equivalent. Todo: add to history box before exiting. */
  if ((statement_type == TOKEN_KEYWORD_QUIT)
  ||  (statement_type == TOKEN_KEYWORD_EXIT))
  {
    action_exit();
    return 1;
  }

  /*
     CLEAR or \c: mysql equivalent.
     Really this just means "ignore the statement".
     It goes into history though.
  */
  if (statement_type == TOKEN_KEYWORD_CLEAR)
  {
    return 1;
  }

  /* USE or \u: mysql equivalent. */
  if (statement_type == TOKEN_KEYWORD_USE)
  {
    QString s;
    int mysql_select_db_result;
    if ((i2 >= 2) && (sub_token_types[0] == TOKEN_KEYWORD_USE)) s= text.mid(sub_token_offsets[1], sub_token_lengths[1]);
    else if (i2 >= 3) s= text.mid(sub_token_offsets[2], sub_token_lengths[2]);
    else
    {
      statement_edit_widget->result= tr("Error. USE statement has no argument.");
      return 1;
    }
    /* If database name is in quotes or delimited, strip. Todo: stripping might be necessary in lots of cases. */
    s= connect_stripper(s);
    int query_len= s.toUtf8().size();                  /* See comment "UTF8 Conversion" */
    char *query= new char[query_len + 1];
    memcpy(query, s.toUtf8().constData(), query_len + 1);

    mysql_select_db_result= mysql_select_db(&mysql[MYSQL_MAIN_CONNECTION], query);
    delete []query;
    if (mysql_select_db_result != 0) put_diagnostics_in_result();
    else
    {
      statement_edit_widget->dbms_database= s;
      statement_edit_widget->result= tr("Database changed");
    }
    return 1;
  }

  /* SOURCE or \.: mysql equivalent. */
  if (statement_type == TOKEN_KEYWORD_SOURCE)
  {
    /* Everything as far as statement end is source file name. */
    /* Todo: if we fill up the line, return an overflow message,
       or make line[] bigger and re-read the file. */
    /* Executing the source-file statements is surprisingly easy: just put them in
       the statement widget. This should be activating action_statement_edit_widget_changed
       and that ultimately causes execution. Handling multiple statements per line is okay.
       Difference from mysql client: this puts source-file statements in history, mysql client puts "source" statement. */
    /* Todo: stop recursion i.e. source statement within source statement. That's an error. */
    QString s;
    unsigned statement_length= /* text.size() */ true_text_size;
    if (i2 >= 2) s= text.mid(sub_token_offsets[1], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    else
    {
      statement_edit_widget->result= tr("Error, SOURCE statement has no argument");
      return 1;
    }
    int query_len= s.toUtf8().size();                  /* See comment "UTF8 Conversion" */
    char *query= new char[query_len + 1];
    memcpy(query, s.toUtf8().constData(), query_len + 1);
    query[query_len]= '\0';                            /* todo: think: is this necessary? */
    FILE *file= fopen(query, "r");                     /* Open specified file, read only */
    delete []query;
    if (file == NULL)
    {
      statement_edit_widget->result= tr("Error, fopen failed");
      return 1;
    }
    char line[2048];
    while(fgets(line, sizeof line, file) != NULL)
    {
      QString source_line= line;
      statement_edit_widget->insertPlainText(source_line);
    }
      fclose(file);
      return 1;
    }

  /* PROMPT or \R: mysql equivalent. */
  /* This overrides the default setting which is made from ocelot_prompt during connect. */
  if (statement_type == TOKEN_KEYWORD_PROMPT)
  {
    QString s;
    unsigned statement_length= true_text_size /* text.size() */;
    if ((i2 >= 2) && (sub_token_types[1] != TOKEN_KEYWORD_USE))
      s= text.mid(sub_token_offsets[1], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    else if (i2 >= 3) s= text.mid(sub_token_offsets[2], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    else
    {
      statement_edit_widget->prompt_as_input_by_user= statement_edit_widget->prompt_default;
      /* Todo: output a message */
      return 1;
    }
    statement_edit_widget->prompt_as_input_by_user= s;
    statement_edit_widget->result= tr("OK");
    return 1;
  }

  /* WARNINGS or \W: mysql equivalent. This overrides a command-line option. */
  if (statement_type == TOKEN_KEYWORD_WARNINGS)
  {
    ocelot_history_includes_warnings= 1;
    statement_edit_widget->result= tr("OK");
    return 1;
  }

  /* NOWARNING or \w: mysql equivalent. This overrides a command-line option. */
  if (statement_type == TOKEN_KEYWORD_NOWARNING)
  {
    ocelot_history_includes_warnings= 0;
    statement_edit_widget->result= tr("OK");
    return 1;
  }

  /* DELIMITER or \d: mysql equivalent. */
  if (statement_type == TOKEN_KEYWORD_DELIMITER)
  {
    if (sub_token_lengths[1] == 0)
    {
      statement_edit_widget->result= tr("Error, delimiter should not be blank");
      return 1;
    }
    /* delimiter = rest of string except lead/trail whitespace */
    QString s;
    unsigned statement_length= true_text_size /* text.size() */;
    s= text.mid(sub_token_offsets[1], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
//        if (s.contains("/")) {
//            statement_edit_widget->result= tr("Error, delimiter should not contain slash");
//            return 1; }
    ocelot_delimiter_str= s.trimmed(); /* Todo: probably trimmed() isn't necessary, but make sure */
    statement_edit_widget->result= tr("OK");
    return 1;
    }

  /* Todo: the following are placeholders, we want actual actions like what mysql would do. */
  if (statement_type == TOKEN_KEYWORD_QUESTIONMARK)
  {
    statement_edit_widget->result= tr("HELP is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_CHARSET)
  {
    statement_edit_widget->result= tr("CHARSET is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_EDIT)
  {
    statement_edit_widget->result= tr("EDIT is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_EGO)
  {
    statement_edit_widget->result= tr("EGO does nothing unless it's on its own line after an executable statement, and --named-commands is true.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_GO)
  {
    statement_edit_widget->result= tr("GO does nothing unless it's on its own line after an executable statement, and --named-commands is true.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_HELP)
  {
    statement_edit_widget->result= tr("HELP is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_NOPAGER)
  {
    statement_edit_widget->result= tr("NOPAGER is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_NOTEE) /* for tee */
  {
    history_tee_stop();
    statement_edit_widget->result= tr("OK");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_PAGER)
  {
    statement_edit_widget->result= tr("PAGER is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_PRINT)
  {
    statement_edit_widget->result= tr("PRINT is not implemented.");
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_REHASH)   /* This overrides a command-line option */
  {
    char result[32];
    sprintf(result, "OK, result set size = %d", rehash_scan());
    ocelot_auto_rehash= 1;
    statement_edit_widget->result= result;
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_STATUS)
  {
    if (is_mysql_connected != 1) statement_edit_widget->result= tr("not connected.");
    else
    {
      QString s;
      statement_edit_widget->result= "";
      s= "DBMS version = " + statement_edit_widget->dbms_version;
      statement_edit_widget->result.append(s);
      s= " Host = " + statement_edit_widget->dbms_host;
      statement_edit_widget->result.append(s);
      s= " Port = " + statement_edit_widget->dbms_port.toUtf8();
      statement_edit_widget->result.append(s);
    }
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_SYSTEM)
  {
    /*
      With mysql client "system ls" would do an ls with system. We use popen not system.
      I don't know whether there is a Windows equivalent; mysql client doesn't support one.
      So the easiest thing for a Windows port is:
      statement_edit_widget->result= tr("SYSTEM is not implemented.");
      Todo: allow "kill" -- some research required about how to stop a shell command.
      Todo: reconsider: maybe output should go to result grid rather than history.
    */
    QString s;
    unsigned statement_length= true_text_size /* text.size() */;
    s= text.mid(sub_token_offsets[1], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    char *command_string= new char[s.size() + 1];
    memcpy(command_string, s.toUtf8().constData(), s.size() + 1);
    statement_edit_widget->result= ""; /* unnecessary? */
    FILE *fp;
    int status;
    char result_line[STRING_LENGTH_512]; /* arbitrary maximum expected line length */
    fp= popen(command_string, "r");
    if (fp == NULL) statement_edit_widget->result.append(tr("popen() failed"));
    else
    {
      while (fgets(result_line, STRING_LENGTH_512, fp) != NULL)
      {
        statement_edit_widget->result.append(result_line);
      }
      status= pclose(fp);
      if (status == -1) statement_edit_widget->result.append(tr("pclose() failed"));
      /* We do not bother to check whether the command failed, display will be blank. */
    }
    delete [] command_string;
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_TEE) /* for tee */
  {
    if (ocelot_history_tee == true)
    {
      statement_edit_widget->result= tr("TEE is already done.");
      return 1;
    }
    /* Everything as far as statement end is tee file name. Compare how we do SOURCE file name. */
    QString s;
    unsigned statement_length= /* text.size() */ true_text_size;
    if (i2 >= 2) s= text.mid(sub_token_offsets[1], statement_length - (sub_token_offsets[1] - sub_token_offsets[0]));
    else
    {
      statement_edit_widget->result= tr("Error, TEE statement has no argument");
      return 1;
    }
    if (history_tee_start(s) == 0) statement_edit_widget->result= tr("Error, fopen failed");
    statement_edit_widget->result= tr("OK");
    return 1;
  }
#ifdef DEBUGGER
  if (statement_type == TOKEN_KEYWORD_DEBUG_DEBUG)
  {
    debug_debug_go(text);
    return 1;
  }
  if ((statement_type == TOKEN_KEYWORD_DEBUG_BREAKPOINT)
   || (statement_type == TOKEN_KEYWORD_DEBUG_CLEAR)
   || (statement_type == TOKEN_KEYWORD_DEBUG_TBREAKPOINT))
  {
    debug_breakpoint_or_clear_go(statement_type, text);
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_SETUP)
  {
    debug_setup_go(text);
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_INSTALL)
  {
    debug_install_go();
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_EXIT)
  {
    debug_exit_go(0);
    debug_menu_enable_or_disable(TOKEN_KEYWORD_DEBUG_EXIT);
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_DELETE)
  {
    debug_delete_go();
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_EXECUTE)
  {
    debug_execute_go();
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_SKIP)
  {
    debug_skip_go();
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_SET)
  {
    debug_set_go(text);
    return 1;
  }
  if (statement_type == TOKEN_KEYWORD_DEBUG_SOURCE)
  {
    debug_source_go();
    return 1;
  }
  if ((statement_type == TOKEN_KEYWORD_DEBUG_CONTINUE)
   || (statement_type == TOKEN_KEYWORD_DEBUG_EXECUTE)
   || (statement_type == TOKEN_KEYWORD_DEBUG_INFORMATION)
   || (statement_type == TOKEN_KEYWORD_DEBUG_LEAVE)
   || (statement_type == TOKEN_KEYWORD_DEBUG_NEXT)
   || (statement_type == TOKEN_KEYWORD_DEBUG_REFRESH)
   || (statement_type == TOKEN_KEYWORD_DEBUG_STEP))
  {
    /* All the other debug commands go to the same place. */
    debug_other_go(text);
    return 1;
  }
#endif
  /* See whether general format is SET @ocelot_... = value ;". Read comment with label = "client variables" */
  if (i2 >= 4)
  {
    if (sub_token_types[0] == TOKEN_KEYWORD_SET)
    {
      /* Todo: figure out why sometimes we say connect_stripper() and sometimes we don't */
      /* Todo: make @ocelot_grid_detached a keyword so you can compare sub_token_types[i] instead. */
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@OCELOT_GRID_DETACHED", Qt::CaseInsensitive) == 0)
      {
        /* Todo: we should compare the rest of the tokens too. */
        is_client_format= 1;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_background_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_background_color= text.mid(sub_token_offsets[3], sub_token_lengths[3]);
        make_style_strings();
        statement_edit_widget->setStyleSheet(ocelot_statement_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_border_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_border_color= text.mid(sub_token_offsets[3], sub_token_lengths[3]);
        make_style_strings();
        statement_edit_widget->setStyleSheet(ocelot_statement_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_font_family", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_font_family= text.mid(sub_token_offsets[3], sub_token_lengths[3]);
        make_style_strings();
        statement_edit_widget->setStyleSheet(ocelot_statement_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_font_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_font_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        statement_edit_widget->setStyleSheet(ocelot_statement_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_font_style", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_font_style= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        statement_edit_widget->setStyleSheet(ocelot_statement_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_font_weight", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_font_weight= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        statement_edit_widget->setStyleSheet(ocelot_statement_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_highlight_literal_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_highlight_literal_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_highlight_identifier_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_highlight_identifier_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_highlight_comment_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_highlight_comment_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_highlight_operator_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_highlight_operator_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_highlight_reserved_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_highlight_reserved_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_statement_prompt_background_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_statement_prompt_background_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        statement_edit_widget->statement_edit_widget_left_bgcolor= QColor(ocelot_statement_prompt_background_color);
        return 2;
      }
      bool is_result_grid_style_changed= false;
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_color= text.mid(sub_token_offsets[3], sub_token_lengths[3]);
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_border_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_border_color= text.mid(sub_token_offsets[3], sub_token_lengths[3]);
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_background_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_background_color= text.mid(sub_token_offsets[3], sub_token_lengths[3]);
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_header_background_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_header_background_color= text.mid(sub_token_offsets[3], sub_token_lengths[3]);
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_font_family", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_font_family= text.mid(sub_token_offsets[3], sub_token_lengths[3]);
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_font_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_font_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_font_style", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_font_style= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_font_weight", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_font_weight= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_cell_border_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_cell_border_color= text.mid(sub_token_offsets[3], sub_token_lengths[3]);
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_cell_right_drag_line_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_cell_right_drag_line_color= text.mid(sub_token_offsets[3], sub_token_lengths[3]);
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_border_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_border_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_cell_border_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_cell_border_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        is_result_grid_style_changed= true;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_grid_cell_right_drag_line_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_grid_cell_right_drag_line_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        is_result_grid_style_changed= true;
      }
      if (is_result_grid_style_changed == true)
      {
        make_style_strings();
        for (int i_r= 0; i_r < RESULT_GRID_TAB_WIDGET_MAX; ++i_r)
        {
          result_grid_table_widget[i_r]->set_all_style_sheets(ocelot_grid_style_string);
        }
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_history_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_history_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_history_background_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_history_background_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_history_border_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_history_border_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_history_font_family", Qt::CaseInsensitive) == 0)
      {
        ocelot_history_font_family= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_history_font_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_history_font_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_history_font_style", Qt::CaseInsensitive) == 0)
      {
        ocelot_history_font_style= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_history_font_weight", Qt::CaseInsensitive) == 0)
      {
        ocelot_history_font_weight= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        history_edit_widget->setStyleSheet(ocelot_history_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_main_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_main_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        main_window->setStyleSheet(ocelot_main_style_string);
        ui->menuBar->setStyleSheet(ocelot_main_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_main_background_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_main_background_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        main_window->setStyleSheet(ocelot_main_style_string);
        ui->menuBar->setStyleSheet(ocelot_main_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_main_border_color", Qt::CaseInsensitive) == 0)
      {
        ocelot_main_border_color= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        main_window->setStyleSheet(ocelot_main_style_string);
        ui->menuBar->setStyleSheet(ocelot_main_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_font_family", Qt::CaseInsensitive) == 0)
      {
        ocelot_main_font_family= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        main_window->setStyleSheet(ocelot_main_style_string);
        ui->menuBar->setStyleSheet(ocelot_main_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_main_font_size", Qt::CaseInsensitive) == 0)
      {
        ocelot_main_font_size= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        main_window->setStyleSheet(ocelot_main_style_string);
        ui->menuBar->setStyleSheet(ocelot_main_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_main_font_style", Qt::CaseInsensitive) == 0)
      {
        ocelot_main_font_style= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        main_window->setStyleSheet(ocelot_main_style_string);
        ui->menuBar->setStyleSheet(ocelot_main_style_string);
        return 2;
      }
      if (QString::compare(text.mid(sub_token_offsets[1], sub_token_lengths[1]), "@ocelot_main_font_weight", Qt::CaseInsensitive) == 0)
      {
        ocelot_main_font_weight= connect_stripper(text.mid(sub_token_offsets[3], sub_token_lengths[3]));
        make_style_strings();
        main_window->setStyleSheet(ocelot_main_style_string);
        ui->menuBar->setStyleSheet(ocelot_main_style_string);
        return 2;
      }
    }
  }

  if (is_client_format == 1)
  {
    /*
      Todo: Here is where we should make a new window. See earlier comments about detaching.
      But the code is old, probably doesn't work any more, and is therefore commented out.
      Todo: consider: should the title bar contain a title?
      Todo: consider alternative non-docking method.
        addWidget                      <- to new window
        setvisible(true)
        new window->show()
        You still have the problem that it won't be at the front, and will be hidden if main window is maximized.
        But there's a way to tell Qt to put something at the front. Look up: raise().
    */
    //test_dock= new QDockWidget(tr(""), this);
    //test_dock->setWidget(statement_edit_widget);
    //addDockWidget(Qt::BottomDockWidgetArea, test_dock);
    //test_dock->show();
    //return 1;
    //statement_edit_widget->hide();                       /* ?? or maybe I should be saying setvisible(false) */
    //main_layout->removeWidget(statement_edit_widget);
    //statement_edit_widget->setParent(0);           /* ?? does removeWidget automatically remove parent anyway? */
    ///* Actually I should be making this a component of a new window, no? */
    //statement_edit_widget->show();
    //return 1;
  }

  return 0;
}


/*
** --auto-rehash or --no-auto-rehash or --skip-auto-rehash. default = TRUE
** unsigned short ocelot_auto_rehash
** Add it to Options menu
** For TOKEN_KEYWORD_REHASH, change ocelot_auto_rehash
** Use scan_rows to make a copy of database/table/column names for current:
   if connect, if reconnect, if "rehash", if "use"
** When user hits \t i.e. tab, if there's an unambiguous name, fill it in
   (don't worry about what happens if user hits tab twice)
** There's also the little matter of whether names are case-sensitive.
They use a hash, we don't.
They are for current database, we're for all (if qualifier). (No, that's doomed due to correlations / aliases.)
In fact we'd like to keep lots of information about columns, for hovering.
*/

int MainWindow::rehash_scan()
{
  MYSQL_RES *res= NULL;
//  QString s;
  unsigned long *grid_max_column_widths;                     /* dynamic-sized list of actual maximum widths in detail columns */

  grid_max_column_widths= 0;

  /* garbage_collect from the last rehash, if any */
  if (rehash_mysql_res_copy != 0) { delete [] rehash_mysql_res_copy; rehash_mysql_res_copy= 0; }
  if (rehash_mysql_res_copy_rows != 0) { delete [] rehash_mysql_res_copy_rows; rehash_mysql_res_copy_rows= 0; }
  rehash_result_column_count= 0;
  rehash_result_row_count= 0;

  if (is_mysql_connected != 1) return 0; /* unexpected_error= "not connected"; */

  if (mysql_query(&mysql[MYSQL_MAIN_CONNECTION],
                  "select table_name, column_name from information_schema.columns where table_schema = database()"))
    {
      return 0; /* unexpected_error= "select failed"; */
    }

  res= mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
  if (res == NULL) return 0; /* unexpected_error= "mysql_store_result failed"; */

  rehash_result_column_count= mysql_num_fields(res);
  rehash_result_row_count= mysql_num_rows(res);
  grid_max_column_widths= new long unsigned int[rehash_result_column_count];
  result_grid_table_widget[0]->scan_rows(
          true, /* mysql_more_results_flag, */
          rehash_result_column_count, /* result_column_count, */
          rehash_result_row_count, /* result_row_count, */
          res, /* grid_mysql_res, */
          &rehash_mysql_res_copy,
          &rehash_mysql_res_copy_rows,
          &grid_max_column_widths);

  mysql_free_result(res);

  delete [] grid_max_column_widths;

  return rehash_result_row_count;
}

/*
  Pass: search string. Return: column name matching searching string.
  Todo: We only look at column[1] column_name. We should look at column[0] table_name.
*/
void MainWindow::rehash_search(char *search_string)
{
  long unsigned int r;
  char *row_pointer;
  unsigned long column_length;
  unsigned int i;
  char column_value[512];
  unsigned int search_string_length;

  search_string_length= strlen(search_string);
  column_value[0]= '\0';

  for (r= 0; r < rehash_result_row_count; ++r)
  {
    row_pointer= rehash_mysql_res_copy_rows[r];
    for (i= 0; i < rehash_result_column_count; ++i)
    {
      memcpy(&column_length, row_pointer, sizeof(column_length));
      row_pointer+= sizeof(column_length);
      /* Now row_pointer points to contents, length has # of bytes */
      if (i == 1)
      {
        if (search_string_length < column_length)
        {
          if (strncmp(row_pointer, search_string, search_string_length) == 0)
          {
            if (column_value[0] != '\0')
            {
              strcpy(search_string, "");
              return; /* too many matches */
            }
            strncpy(column_value, row_pointer, column_length);
            column_value[column_length]= '\0';
          }
        }
      }
    row_pointer+= column_length;
    }
  }
  strcpy(search_string, column_value);
}

/*
  Call widget_sizer() from action_execute() to resize the three
  main_layout widgets if necessary.
  Todo: so far widget_sizer() is just ideas, mostly unimplemented.
  The widgets are in a layout (I don't think it's possible to have them in a layout if one uses dock windows).
  But the maximum sizing can be helped by me. Some rules are:
  * If history_edit_widget is higher than needed without scrolling, shrink it.
  * If user has manually expanded a window, leave it.
    ** But a grid should snap back to a minimum size
  * A select() should be visible, up to some maximum line count, even if a result set has many rows.
  * We need a scroll bar for the whole main window. But maybe not for all the main_layout widgets?
  * The widget might be undocked -- in that case the sizes of other docked widgets won't matter.
  * The statement window can shrink quite a bit because after action_execute() there's nothing in it.
  Probably TitleBarHeight won't matter because we set the bar to vertical (update: no, I gave up on vertical)
  Probably PM_DefaultFrameWidth won't matter but I'm not sure what it is.
  Probably there is no space between dock windows. ... Update: no, look at it, there is a space.
  So far all I've thought about is a special case for history_dock.
    It's a bit silly to allow for height of a horizontal scroll bar, if there is no scroll bar.
  Maybe we should use the WA_DontShowOnScreen trick.
*/
void MainWindow::widget_sizer()
{
  int history_edit_widget_height;
  /*
    This would work, but only if show() is called before widget_sizer().
    But if history_edit_widget becomes something other than plain text,
    we'd have to do something like this ...
    history_edit_widget_height= history_edit_widget->document()->size().height();
    history_edit_widget->setMaximumHeight(history_edit_widget_height);
  */
  QFontMetrics history_font(history_edit_widget->font());
  history_edit_widget_height= (history_edit_widget->document()->lineCount() + 1) * history_font.lineSpacing();
  history_edit_widget_height+= QApplication::style()->pixelMetric(QStyle::PM_ScrollBarExtent);
  history_edit_widget->setMaximumHeight(history_edit_widget_height);
}


/*
  If the response to a MySQL query is an error, put error information
  in statement_edit_widget->result and return error number. Else return 0.
  This should only be called for mysql calls, not for client commands.
*/
/*
  Todo: the elapsed-time calculation in the diagnostics is calculated at the wrong time,
  it includes the time to set up the widgets. Move it to just after statement-execution.
*/
void MainWindow::put_diagnostics_in_result()
{
  unsigned int mysql_errno_result;
  char mysql_error_and_state[50]; /* actually we should need less than 50 */
  QString s1, s2;

  mysql_errno_result= mysql_errno(&mysql[MYSQL_MAIN_CONNECTION]);
  if (mysql_errno_result == 0)
  {
    s1= tr("OK");

    /* This should output, e.g. "Records: 3 Duplicates: 0 Warnings: 0" -- but actually nothing happens. */
    if (mysql_info(&mysql[MYSQL_MAIN_CONNECTION])!=NULL)
    {
      /* This only works for certain insert, load, alter or update statements */
      s1.append(tr(mysql_info(&mysql[MYSQL_MAIN_CONNECTION])));
    }
    else
    {
      sprintf(mysql_error_and_state, " %llu rows affected", mysql_affected_rows(&mysql[MYSQL_MAIN_CONNECTION]));
      s1.append(mysql_error_and_state);
    }
    //printf("info=%s.\n", mysql_info(&mysql));
    /* Add to display: how long the statement took, to nearest tenth of a second. Todo: fix calculation. */
    qint64 statement_end_time= QDateTime::currentMSecsSinceEpoch();
    qint64 elapsed_time= statement_end_time - statement_edit_widget->start_time;
    long int elapsed_time_as_long_int= (long int) elapsed_time;
    float elapsed_time_as_float= (float) elapsed_time_as_long_int / 1000;
    sprintf(mysql_error_and_state, "(%.1f seconds)", elapsed_time_as_float);
    s1.append(mysql_error_and_state);
    if (mysql_warning_count(&mysql[MYSQL_MAIN_CONNECTION]) > 0)
    {
      if (ocelot_history_includes_warnings > 0)
      {
        mysql_query(&mysql[MYSQL_MAIN_CONNECTION], "show warnings");
        MYSQL_RES *mysql_res_for_warnings;
        MYSQL_ROW warnings_row;
        QString s;
        // unsigned long connect_lengths[0];
        mysql_res_for_warnings= mysql_store_result(&mysql[MYSQL_MAIN_CONNECTION]);
        for (unsigned int wi= 0; wi <= mysql_warning_count(&mysql[MYSQL_MAIN_CONNECTION]); ++wi)
        {
          warnings_row= mysql_fetch_row(mysql_res_for_warnings);
          if (warnings_row != NULL)
          {
            /* lengths= mysql_fetch_lengths(connect_res); */
            sprintf(mysql_error_and_state, "\n%s (%s) %s.", warnings_row[0], warnings_row[1], warnings_row[2]);
            s1.append(mysql_error_and_state);
          }
        }
        mysql_free_result(mysql_res_for_warnings);
      }
    }
  }
  if (mysql_errno_result > 0)
  {
    s1= tr("Error ");
    sprintf(mysql_error_and_state, "%d (%s) ", mysql_errno_result, mysql_sqlstate(&mysql[MYSQL_MAIN_CONNECTION]));
    s1.append(mysql_error_and_state);
    s2= mysql_error(&mysql[MYSQL_MAIN_CONNECTION]);
    s1.append(s2);
  }
  statement_edit_widget->result= s1;
}


/*
   tokenize(): Produce a list of tokens given an SQL statement using MySQL rules.

   Example: SELECT `a` FROM t; will give:
     token_offsets[0] -> SELECT, token_lengths=0] = 6
     token_offsets[1] -> `a`, token_lengths[1] = 3
     token_offsets[2] -> FROM, token_lengths[2] = 4
     token_offsets[3] -> t, token_lengths[3] = 1
     token_offsets[4] -> ;, token_lengths[4] = 1
     token_offsets[5] -> `'0', token_lengths[5] = 0
   Most character sets are OK because all breaks are ASCII.

  What to pass:
    Token_lengths[] gets the length of the token
    Token_offsets[] gets where the token starts within text
    Max_tokens is the size of token_lengths[] and token_offsets
      This is what I actually depend on but C++ insists I state the size.
    Version is what to compare when you say / * ! nnnnn
      It must be a 5-character string.
      It will be compared with the 5 digits following '!', if any
      If comparison fails, / * ! is treated as / * style comment
    Comment behaviour influences what will happen with comments
      1: save whole comment as one token.
         useful for syntax highlighting.
      2: ignore whole comment.
         useful for debug.
      3: save whole comment as multiple tokens, include markers
         useful for html markup.
      4: save whole comment as multiple tokens, ignore markers
    Special_token is a string, possibly blank, which will be treated as a single n-byte token
    even if it contains special characters. In this program we use it for delimiters.
    Minus behaviour influences what will happen with '-' i.e. 0x2d i.e. hyphen-minus
      1: '-' is an operator, this is the norm when tokenizing SQL
      2: '-' is a token part, this is the norm when tokenizing options

  Adjusting the tokenizer for Qt:
    With tokenize(QString text, char* comment_handling) we find the tokens of a string in statement.
    The results go to token_offsets[] and token_lengths[].
    According to Qt documentation:
      QString contains QChars. It's really UTF-16 except that a surrogate pair counts as two QChars.
      I won't use Qchar's helpers (isHighSurrogate, isLowSurrogate, isDigit, isLetter, isSpace, isNull)
      because I can't be sure that MySQL/MariaDB parser will always act the same way.
      I'm assuming nobody has played with the exec's codec, whatever that means.
      I'm assuming that the compiler will treat each QChar as unsigned.
      So I know that if something is "QString str" then I can compare characters within it to
      "QChar('u')" or "QChar(0x00e7)" etc. and I can get substrings with str.mid(position,length).
  The original program is tokenize.c which is a standalone that uses unsigned char* not QString.
*/

void MainWindow::tokenize(QChar *text, int text_length, int (*token_lengths)[MAX_TOKENS],
                           int (*token_offsets)[MAX_TOKENS], int max_tokens,QChar *version,
                           int passed_comment_behaviour, QString special_token, int minus_behaviour)
{
  int token_number;
  QChar expected_char;
  int char_offset;
  QChar expected_char_at_comment_end;
  int comment_behaviour;
  QChar version_inside_comment[6];
  int n;

  expected_char_at_comment_end=' ';
  comment_behaviour=passed_comment_behaviour;
  token_number= 0;
  char_offset= 0;
next_token:
  (*token_lengths)[token_number]= 0;
  (*token_offsets)[token_number]= char_offset;
next_char:
  if (token_number >= (max_tokens - 1)) goto string_end;
  /* Following IFs happen to be in order by ASCII code */
  if (char_offset >= text_length) goto string_end; /* this is the usual check for end of string */
  if (text[char_offset] == 0) goto string_end;  /* \0 end of string (probably will never happen) */
  /* Check whether special_token occurs at this point. */
  {
    int special_token_length, special_token_offset;
    special_token_length= special_token.length();
    if ((special_token_length > 0)
    &&  (char_offset + special_token_length <= text_length))
    {
      for (special_token_offset= 0; special_token_offset < special_token_length; ++special_token_offset)
      {
        QChar special_token_char= special_token.at(special_token_offset);
        if (special_token_char != text[char_offset+special_token_offset]) break;
      }
      if (special_token_offset == special_token_length)
      {
        n= special_token_length;
        goto n_byte_token;
      }
    }
  }
  if (text[char_offset] <= ' ') goto white_space; /* space tab ff etc. = white space */
  if (text[char_offset] == '!')                 /* ! might be start of != otherwise one-byte token */
  {
    expected_char= '=';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == '"')    /* " starts a token until next " but watch for ""s and end-of-string and escape */
  {
    expected_char= '"';
    goto skip_till_expected_char;
  }
  if (text[char_offset] == '#')          /* # starts a comment */
  {
    expected_char= '\n';
    goto comment_start;
  }
  if (text[char_offset] == '%') goto one_byte_token; /* % one-byte token */
  if (text[char_offset] == '&')     /* & might be start of &&. otherwise one-byte token */
  {
    expected_char= '&';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == 39)     /* ' starts a token until next ' but watch for end-of-string and escape */
  {
    expected_char= 39;
    goto skip_till_expected_char;
  }
  if (text[char_offset] == '(') goto one_byte_token; /* ( one-byte token */
  if (text[char_offset] == ')') goto one_byte_token; /* ) one-byte token */
  if (text[char_offset] == '*') /* * one-byte token unless * / and we're inside a comment */
  {
    if ((expected_char_at_comment_end == '/') && (char_offset + 1 < text_length) && (text[char_offset + 1] == '/'))
    {
      expected_char_at_comment_end= ' ';
      if (comment_behaviour == 3)
      {
        expected_char= '/';
        goto skip_till_expected_char;
      }
      if (comment_behaviour == 4)
      {
        n= 2;
        goto n_byte_token_skip;
      }
    }
    else goto one_byte_token;
  }
  if (text[char_offset] == '+') goto one_byte_token; /* + one-byte token */
  if (text[char_offset] == ',') goto one_byte_token; /* , one-byte token */
  if (minus_behaviour != 2) {
    if (text[char_offset] == '-')     /* - one-byte token, unless -- comment */
    {
      if ((char_offset + 1 < text_length) && (text[char_offset + 1]  == '-'))
      {
        if ((char_offset + 2 < text_length) && (text[char_offset + 2] <= ' '))
        {
          expected_char= '\n';
          goto comment_start;
        }
      }
      goto one_byte_token;
    }
  }
  if (text[char_offset] == '.')     /* . part of token if previous or following is digit. otherwise one-byte token */
  {
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] >= '0') && (text[char_offset + 1] <= '9')) goto part_of_token;
    if ((*token_lengths)[token_number] > 0)
    {
      if ((text[char_offset - 1] >= '0') && (text[char_offset - 1] <= '9')) goto part_of_token;
    }
    goto one_byte_token;
  }
  if (text[char_offset] == '/') /* / one-byte token unless it's '/' followed by '*' */
  {
    expected_char= '/';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == '*')) goto comment_starting_with_slash_start;
    goto one_byte_token;
  }
  if ((text[char_offset] >= '0') && (text[char_offset] <= '9')) goto part_of_token; /* digit part of token */
  if (text[char_offset] == ':')    /* : might be start of := otherwise one-byte token */
  {
    expected_char= '=';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == ';') goto one_byte_token; /* ; one-byte token */
  if (text[char_offset] == '<')       /* < might be start of <=  or << or <=> or <>. otherwise one-byte token */
  {
    expected_char= '=';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char))
    {
      if ((char_offset + 2 < text_length) && (text[char_offset + 2] == '>'))
      {
        expected_char= '>';
        goto skip_till_expected_char;
      }
      goto skip_till_expected_char;
    }
    expected_char= '>';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    expected_char= '<';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == '=') goto one_byte_token; /* = one-byte token */
  if (text[char_offset] == '>')   /* > might be start of >= or >>. otherwise one-byte token */
  {
    expected_char= '=';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    expected_char= '>';
    if ((char_offset + 1  < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == '?') goto one_byte_token; /* ? one-byte token? */
  if (text[char_offset] == '@') /* @ start of @token or start of @@token otherwise one-byte token */
  {
    if ((*token_lengths)[token_number] == 0) goto part_of_token;
    if ((*token_lengths)[token_number] == 1)
    {
      if (text[char_offset - 1] == '@') goto part_of_token;
    }
    goto one_byte_token;
  }
  if (text[char_offset] == '[') goto one_byte_token; /* [ one-byte token which is never used */
  if (text[char_offset] == 92)  goto one_byte_token; /* \ one-byte token which is never used */
  if (text[char_offset] == ']') goto one_byte_token; /* ] one-byte token which is never used */
  if (text[char_offset] == '^') goto one_byte_token; /* ^ one-byte token */
  if (text[char_offset] == '`')        /* ` starts a token until next ` but watch for end-of-string and escape */
  {
    expected_char= '`';
    goto skip_till_expected_char;
  }
  if (text[char_offset] == '{')      /* { starts a token until next } but watch for end-of-string */
  {
    expected_char= '}';
    goto skip_till_expected_char;
  }
  if (text[char_offset] == '|')      /* | might be start of ||. otherwise one-byte token */
  {
    expected_char= '|';
    if ((char_offset + 1 < text_length) && (text[char_offset + 1] == expected_char)) goto skip_till_expected_char;
    goto one_byte_token;
  }
  if (text[char_offset] == '}') goto one_byte_token; /* } one-byte token which is never used */
  if (text[char_offset] == '~') goto one_byte_token; /* ~ one-byte token */
  /* Remaining possibilities are:
     $, 0 to 9, A to Z, a to z
     >127 (top-bit-on is probably a continuation byte of a utf-8 character)
     _ DEL */
part_of_token:
  ++char_offset;
  ++(*token_lengths)[token_number];
  goto next_char;
string_end:
  if ((*token_lengths)[token_number] > 0) (*token_lengths)[token_number + 1]= 0;
  return;
white_space:
  if ((*token_lengths)[token_number] > 0) ++token_number;
  ++char_offset;
  goto next_token;
comment_starting_with_slash_start:
comment_start:
  comment_behaviour= passed_comment_behaviour;

  if (text[char_offset] == '/')
  {
    if ((char_offset + 2 < text_length) && (text[char_offset + 2] == '!'))
    {
      if ((char_offset + 8 < text_length) && (text[char_offset + 3] >= '0') && (text[char_offset + 3] <= '9'))
      {
        version_inside_comment[0]= text[char_offset + 3];
        version_inside_comment[1]= text[char_offset + 4];
        version_inside_comment[2]= text[char_offset + 5];
        version_inside_comment[3]= text[char_offset + 6];
        version_inside_comment[4]= text[char_offset + 7];
        version_inside_comment[5]= 0;
      }
      else
      {
        version_inside_comment[0]= 0;
      }
      if (strcmp((char*)version, (char*)version_inside_comment) >= 0)
      {
        if (comment_behaviour == 1) comment_behaviour= 3;
        if (comment_behaviour == 2) comment_behaviour= 4;
        expected_char= ' ';
        expected_char_at_comment_end= '/';
        if (comment_behaviour == 4)
        {
          char_offset+= 3 + strlen((char*)version_inside_comment);
          goto next_token;
        }
        goto skip_till_expected_char;
      }
    }
  }

  if (comment_behaviour == 3)
  {
    expected_char_at_comment_end= expected_char; /* / or \n */
    if (text[char_offset] == '#') goto one_byte_token;
    if (text[char_offset] == '-') expected_char='-';
    if (text[char_offset] == '/') expected_char='*';
  }
  if (comment_behaviour == 4)
  {
    /* comment_behaviour=4 means ignore / * and * / but not comment contents */
    expected_char_at_comment_end= expected_char; /* / or \n */
    if (text[char_offset] == '#') n= 1;
    else n= 2;
    goto n_byte_token_skip;
  }
skip_till_expected_char:
  /* No break if we're facing N'...' or X'...' or B'...' or @`...` or @a"..." */
  /* Todo: check: I might have forgotten N"..." etc. */
  if ((*token_lengths)[token_number] == 1)
  {
    if (expected_char == 39)
    {
      if ((text[char_offset - 1] == 'N') || (text[char_offset - 1] == 'X') || (text[char_offset - 1] == 'B')
              ||(text[char_offset - 1] == 'n') || (text[char_offset - 1] == 'x') || (text[char_offset- 1] == 'b'))
      {
        goto skip_till_expected_char_2;
      }
    }
    if ((expected_char == '`') || (expected_char == '"'))
    {
      if (text[char_offset - 1] == '@')
      {
        goto skip_till_expected_char_2;
      }
    }
  }
  if ((*token_lengths)[token_number] > 0)
  {
    ++token_number;
    (*token_lengths)[token_number]= 0;
    (*token_offsets)[token_number]= char_offset;
  }
skip_till_expected_char_2:
  ++char_offset;
  ++(*token_lengths)[token_number];
  if (char_offset >= text_length) goto string_end;
  if (text[char_offset] == 0) goto string_end;
  if (text[char_offset] != expected_char) goto skip_till_expected_char_2;
  /* No match if / not preceded by *, \' or '' inside ''s, \" or "" inside ""s or `` inside ``s */
  if ((expected_char == '/') && (text[char_offset - 1] != '*')) goto skip_till_expected_char_2;
  if ((expected_char == 39) && (text[char_offset - 1] == 92)) goto skip_till_expected_char_2;
  if ((expected_char == '"') && (text[char_offset - 1] == 92)) goto skip_till_expected_char_2;
  if ((expected_char == 39) || (expected_char == '"') || (expected_char == '`'))
  {
    if ((char_offset + 1 < text_length) && (expected_char == text[char_offset + 1]))
    {
      ++char_offset;
      ++(*token_lengths)[token_number];
      goto skip_till_expected_char_2;
    }
  }
  ++char_offset;
  ++(*token_lengths)[token_number];
  if ((expected_char == '/') || (expected_char == '\n'))
  {
    if (comment_behaviour == 2)
    {
      /*
        comment_behaviour=2 means "ignore comments (treat them as whitespace)",
        so ignore, i.e. go for next token without incrementing token_number.
      */
      goto next_token;
    }
    if (comment_behaviour == 4)
    {
      if (expected_char == expected_char_at_comment_end)
      {
        /*
          comment_behaviour=4 means "don't ignore comments but treat begin/end
          marks as whitespace" so declare there's no more end-marks, then ignore.
        */
        expected_char_at_comment_end= ' ';
        goto next_token;
      }
    }
  }
  ++token_number;
  goto next_token;
one_byte_token: /* we know that coming token length is 1 */
  if ((*token_lengths)[token_number] > 0) ++token_number;
  (*token_offsets)[token_number]= char_offset;
  (*token_lengths)[token_number]= 1;
  ++char_offset;
  ++token_number;
  goto next_token;
n_byte_token:   /* we know that coming token length is n */
  if ((*token_lengths)[token_number] > 0) ++token_number;
  (*token_offsets)[token_number]= char_offset;
  (*token_lengths)[token_number]= n;
  char_offset+= n;
  ++token_number;
  goto next_token;
n_byte_token_skip:
  if ((*token_lengths)[token_number] > 0) ++token_number;
  char_offset+= n;
  goto next_token;
}


/*
  token_type() should be useful for syntax highlighting and for hovering.
  Pass: token, token length. we assume it's at least 1.
  Return: type
  I could have figured this out during tokenize(), but didn't.
  Todo: distinguish between keyword, regular identifier, reserved word.
        There is a check elsewhere for keywords though, see tokens_to_keywords().
*/
int MainWindow::token_type(QChar *token, int token_length)
{
  if (token_length == 0) return TOKEN_TYPE_OTHER;
  /* Check whether token == delimiter_str. similar to a comparison with special_token in tokenize(). */
  {
    int special_token_length, special_token_offset;
    special_token_length= ocelot_delimiter_str.length();
    if ((special_token_length > 0)
    &&  (special_token_length == token_length))
    {
      for (special_token_offset= 0; special_token_offset < special_token_length; ++special_token_offset)
      {
        QChar special_token_char= ocelot_delimiter_str.at(special_token_offset);
        if (special_token_char != *(token+special_token_offset)) break;
      }
      if (special_token_offset == special_token_length)
      {
        return TOKEN_TYPE_OPERATOR;
      }
    }
  }

  if (*token == 39) return TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE;
  if (token_length > 1)
  {
    if ((*token == 'N') && (*(token + 1) == 39)) return TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE;
    if ((*token == 'X') && (*(token + 1) == 39)) return TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE;
    if ((*token == 'B') && (*(token + 1) == 39)) return TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE;
  }
  if (*token == '"') return TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE;
  if ((*token >= '0') && (*token <= '9')) return TOKEN_TYPE_LITERAL_WITH_DIGIT;
  if (*token == '{') return TOKEN_TYPE_LITERAL_WITH_BRACE;
  if (*token == '`') return TOKEN_TYPE_IDENTIFIER_WITH_BACKTICK;
  if (*token == '@') return TOKEN_TYPE_IDENTIFIER_WITH_AT;
  if (token_length > 1)
  {
    if ((*token == '/') && (*(token + 1) == '*')) return TOKEN_TYPE_COMMENT_WITH_SLASH;
    if ((*token == '*') && (*(token + 1) == '/')) return TOKEN_TYPE_COMMENT_WITH_SLASH;
  }
  if (*token == '#') return TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE;
  if (token_length > 1)
  {
    if ((*token == '-') && (*(token + 1) == '-')) return TOKEN_TYPE_COMMENT_WITH_MINUS;
  }
  if ((*token > ' ') && (*token < 'A') && (*token != '$')) return TOKEN_TYPE_OPERATOR;
  return TOKEN_TYPE_OTHER;
}


/*
  I got tired of repeating
  if (QString::compare(text.mid(main_token_offsets[0], main_token_lengths[0]), "SELECT", Qt::CaseInsensitive) == 0) ...
  so switched to maintaining a permanent list.
  Two compiler-dependent assumptions: bsearch() exists, and char* can be converted to unsigned long.
*/
/* Todo: use "const" and "static" more often */
void MainWindow::tokens_to_keywords(QString text)
{
  /*
    Sorted list of keywords.
    If you change this, you must also change bsearch parameters and change TOKEN_KEYWORD list.
  */
  const char strvalues[][30]=
  {
    "?", /* Ocelot keyword, although tokenize() regards it as an operator */
    "ACCESSIBLE",
    "ADD",
    "ALL",
    "ALTER",
    "ANALYZE",
    "AND",
    "AS",
    "ASC",
    "ASCII",
    "ASENSITIVE",
    "BEFORE",
    "BEGIN",
    "BETWEEN",
    "BIGINT",
    "BINARY",
    "BIT",
    "BLOB",
    "BOOL",
    "BOOLEAN",
    "BOTH",
    "BY",
    "CALL",
    "CASCADE",
    "CASE",
    "CHANGE",
    "CHAR",
    "CHARACTER",
    "CHARSET", /* Ocelot keyword */
    "CHECK",
    "CLEAR", /* Ocelot keyword */
    "COLLATE",
    "COLUMN",
    "CONDITION",
    "CONNECT", /* Ocelot keyword */
    "CONSTRAINT",
    "CONTINUE",
    "CONVERT",
    "CREATE",
    "CROSS",
    "CURRENT_DATE",
    "CURRENT_TIME",
    "CURRENT_TIMESTAMP",
    "CURRENT_USER",
    "CURSOR",
    "DATABASE",
    "DATABASES",
    "DATE",
    "DATETIME",
    "DAY_HOUR",
    "DAY_MICROSECOND",
    "DAY_MINUTE",
    "DAY_SECOND",
    "DEC",
    "DECIMAL",
    "DECLARE",
    "DEFAULT",
    "DELAYED",
    "DELETE",
    "DELIMITER", /* Ocelot keyword */
    "DESC",
    "DESCRIBE",
    "DETERMINISTIC",
    "DISTINCT",
    "DISTINCTROW",
    "DIV",
    "DO",
    "DOUBLE",
    "DROP",
    "DUAL",
    "EACH",
    "EDIT", /* Ocelot keyword */
    "EGO", /* Ocelot keyword */
    "ELSE",
    "ELSEIF",
    "ENCLOSED",
    "END",
    "ENUM",
    "ESCAPED",
    "EVENT",
    "EXISTS",
    "EXIT",
    "EXPLAIN",
    "FALSE",
    "FETCH",
    "FLOAT",
    "FLOAT4",
    "FLOAT8",
    "FOR",
    "FORCE",
    "FOREIGN",
    "FROM",
    "FULLTEXT",
    "FUNCTION",
    "GEOMETRY",
    "GEOMETRYCOLLECTION",
    "GET",
    "GO", /* Ocelot keyword */
    "GRANT",
    "GROUP",
    "HAVING",
    "HELP", /* Ocelot keyword */
    "HIGH_PRIORITY",
    "HOUR_MICROSECOND",
    "HOUR_MINUTE",
    "HOUR_SECOND",
    "IF",
    "IGNORE",
    "IN",
    "INDEX",
    "INFILE",
    "INNER",
    "INOUT",
    "INSENSITIVE",
    "INSERT",
    "INT",
    "INT1",
    "INT2",
    "INT3",
    "INT4",
    "INT8",
    "INTEGER",
    "INTERVAL",
    "INTO",
    "IO_AFTER_GTIDS",
    "IO_BEFORE_GTIDS",
    "IS",
    "ITERATE",
    "JOIN",
    "KEY",
    "KEYS",
    "KILL",
    "LEADING",
    "LEAVE",
    "LEFT",
    "LIKE",
    "LIMIT",
    "LINEAR",
    "LINES",
    "LINESTRING",
    "LOAD",
    "LOCALTIME",
    "LOCALTIMESTAMP",
    "LOCK",
    "LOGFILE",
    "LONG",
    "LONGBLOB",
    "LONGTEXT",
    "LOOP",
    "LOW_PRIORITY",
    "MASTER_BIND",
    "MASTER_SSL_VERIFY_SERVER_CERT",
    "MATCH",
    "MAXVALUE",
    "MEDIUMBLOB",
    "MEDIUMINT",
    "MEDIUMTEXT",
    "MIDDLEINT",
    "MINUTE_MICROSECOND",
    "MINUTE_SECOND",
    "MOD",
    "MODIFIES",
    "MULTILINESTRING",
    "MULTIPOINT",
    "MULTIPOLYGON",
    "NATURAL",
    "NONBLOCKING",
    "NOPAGER", /* Ocelot keyword */
    "NOT",
    "NOTEE", /* Ocelot keyword */
    "NOWARNING", /* Ocelot keyword */
    "NO_WRITE_TO_BINLOG",
    "NULL",
    "NUMERIC",
    "ON",
    "OPTIMIZE",
    "OPTION",
    "OPTIONALLY",
    "OR",
    "ORDER",
    "OUT",
    "OUTER",
    "OUTFILE",
    "PAGER", /* Ocelot keyword */
    "PARTITION",
    "POINT",
    "POLYGON",
    "PRECISION",
    "PRIMARY",
    "PRINT", /* Ocelot keyword */
    "PROCEDURE",
    "PROMPT", /* Ocelot keyword */
    "PURGE",
    "QUIT", /* Ocelot keyword */
    "RANGE",
    "READ",
    "READS",
    "READ_WRITE",
    "REAL",
    "REFERENCES",
    "REGEXP",
    "REHASH", /* Ocelot keyword */
    "RELEASE",
    "RENAME",
    "REPEAT",
    "REPLACE",
    "REQUIRE",
    "RESIGNAL",
    "RESTRICT",
    "RETURN",
    "RETURNS",
    "REVOKE",
    "RIGHT",
    "RLIKE",
    "ROW",
    "SCHEMA",
    "SCHEMAS",
    "SECOND_MICROSECOND",
    "SELECT",
    "SENSITIVE",
    "SEPARATOR",
    "SERIAL",
    "SERVER",
    "SET",
    "SHOW",
    "SIGNAL",
    "SMALLINT",
    "SOURCE", /* Ocelot keyword */
    "SPATIAL",
    "SPECIFIC",
    "SQL",
    "SQLEXCEPTION",
    "SQLSTATE",
    "SQLWARNING",
    "SQL_BIG_RESULT",
    "SQL_CALC_FOUND_ROWS",
    "SQL_SMALL_RESULT",
    "SSL",
    "STARTING",
    "STATUS", /* Ocelot keyword */
    "STRAIGHT_JOIN",
    "SYSTEM", /* Ocelot keyword */
    "TABLE",
    "TABLESPACE",
    "TEE", /* Ocelot keyword */
    "TERMINATED",
    "THEN",
    "TIME",
    "TIMESTAMP",
    "TINYBLOB",
    "TINYINT",
    "TINYTEXT",
    "TO",
    "TRAILING",
    "TRIGGER",
    "TRUE",
    "UNDO",
    "UNICODE",
    "UNION",
    "UNIQUE",
    "UNLOCK",
    "UNSIGNED",
    "UPDATE",
    "USAGE",
    "USE", /* Ocelot keyword, also reserved word */
    "USING",
    "UTC_DATE",
    "UTC_TIME",
    "UTC_TIMESTAMP",
    "VALUES",
    "VARBINARY",
    "VARCHAR",
    "VARCHARACTER",
    "VARYING",
    "VIEW",
    "WARNINGS", /* Ocelot keyword */
    "WHEN",
    "WHERE",
    "WHILE",
    "WITH",
    "WRITE",
    "XOR",
    "YEAR",
    "YEAR_MONTH",
    "ZEROFILL"
  };
  //QString text;
  QString s= "";
  int t;
  char *p_item;
  unsigned long index;
  char key2[30 + 1];
  int i, i2;

  //text= statement_edit_widget->toPlainText();
  for (i2= 0; main_token_lengths[i2] != 0; ++i2)
  {
    /* Get the next word. */
    s= text.mid(main_token_offsets[i2], main_token_lengths[i2]);
    t= token_type(s.data(), main_token_lengths[i2]);
    main_token_types[i2]= t;
    if (t == TOKEN_TYPE_OTHER)
    {
      /* It's not a literal or operator. Maybe it's a keyword. Convert it to char[]. */
      QByteArray key_as_byte_array= s.toLocal8Bit();
      const char *key= key_as_byte_array.data();
      /* Uppercase it. I don't necessarily have strupr(). */
      for (i= 0; (*(key + i) != '\0') && (i < 30); ++i) key2[i]= toupper(*(key + i)); key2[i]= '\0';
      /* Search it with library binary-search. Assume 285 items and everything 30 bytes long. */
      p_item= (char*) bsearch (key2, strvalues, 285, 30, (int(*)(const void*, const void*)) strcmp);
      if (p_item != NULL)
      {
        /* It's in the list, so instead of TOKEN_TYPE_OTHER, make it TOKEN_KEYWORD_something. */
        index= ((((unsigned long)p_item - (unsigned long)strvalues)) / 30) + TOKEN_KEYWORDS_START;
         main_token_types[i2]= index;
      }
#ifdef DEBUGGER
      else
      {
        /* It's not in the list, but if it's unambiguously $DEB[UG] etc. then consider it a debug keyword. */
        if ((key2[0] == '$') && (key2[1] != '\0'))
        {
          if (strncmp(key2, "$BREAKPOINT", strlen(key2)) == 0) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_BREAKPOINT;
          if ((strlen(key2) > 2) && (strncmp(key2, "$CLEAR", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_CLEAR;
          if ((strlen(key2) > 2) && (strncmp(key2, "$CONTINUE", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_CONTINUE;
          if ((strlen(key2) > 3) && (strncmp(key2, "$DEBUG", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_DEBUG;
          if ((strlen(key2) > 3) && (strncmp(key2, "$DELETE", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_DELETE;
          if ((strlen(key2) > 3) && (strncmp(key2, "$EXECUTE", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_EXECUTE;
          if ((strlen(key2) > 3) && (strncmp(key2, "$EXIT", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_EXIT;
          if ((strlen(key2) > 3) && (strncmp(key2, "$INFORMATION", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_INFORMATION;
          if ((strlen(key2) > 3) && (strncmp(key2, "$INSTALL", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_INSTALL;
          if (strncmp(key2, "$LEAVE", strlen(key2)) == 0) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_LEAVE;
          if (strncmp(key2, "$NEXT", strlen(key2)) == 0) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_NEXT;
          if (strncmp(key2, "$REFRESH", strlen(key2)) == 0) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_REFRESH;
          if (strcmp(key2, "$SET") == 0) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_SET;
          if ((strlen(key2) > 4) && (strncmp(key2, "$SETUP", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_SETUP;
          if ((strlen(key2) > 2) && (strncmp(key2, "$SKIP", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_SKIP;
          if ((strlen(key2) > 2) && (strncmp(key2, "$SOURCE", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_SOURCE;
          if ((strlen(key2) > 2) && (strncmp(key2, "$STEP", strlen(key2)) == 0)) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_STEP;
          if (strncmp(key2, "$TBREAKPOINT", strlen(key2)) == 0) main_token_types[i2]= TOKEN_KEYWORD_DEBUG_TBREAKPOINT;
        }
      }
#endif
    }
  }

  main_token_count= i2; /* Global. There will be several times when we want to know how many tokens there are. */

  main_token_types[i2]= 0;

  /* The special cases of BEGIN, DO, END, FUNCTION, ROW. */
  int i_of_function= -1;
  int start_of_body= find_start_of_body(text, &i_of_function);

  tokens_to_keywords_revert(start_of_body, i_of_function, text);

  /*
    The special case of "?". Although tokenize() says ? is an operator,
    we might have to treat it as a keyword meaning "help".
  */
  if (main_token_lengths[0] == 1)
  {
    s= text.mid(main_token_offsets[0], 1);
    if (s == QString("?")) main_token_types[0]= TOKEN_KEYWORD_QUESTIONMARK;
  }

  /* Todo: This has to be moved somewhere because the text might contain multi-statements. */
  if ((main_token_types[0] == TOKEN_KEYWORD_QUESTIONMARK)
  ||  (main_token_types[0] == TOKEN_KEYWORD_CHARSET)
  ||  (main_token_types[0] == TOKEN_KEYWORD_CLEAR)
  ||  (main_token_types[0] == TOKEN_KEYWORD_CONNECT)
  ||  (main_token_types[0] == TOKEN_KEYWORD_DELIMITER)
  ||  (main_token_types[0] == TOKEN_KEYWORD_EDIT)
  ||  (main_token_types[0] == TOKEN_KEYWORD_EGO)
  ||  (main_token_types[0] == TOKEN_KEYWORD_GO)
  ||  (main_token_types[0] == TOKEN_KEYWORD_HELP)
  ||  (main_token_types[0] == TOKEN_KEYWORD_NOPAGER)
  ||  (main_token_types[0] == TOKEN_KEYWORD_NOTEE)
  ||  (main_token_types[0] == TOKEN_KEYWORD_NOWARNING)
  ||  (main_token_types[0] == TOKEN_KEYWORD_PAGER)
  ||  (main_token_types[0] == TOKEN_KEYWORD_PRINT)
  ||  (main_token_types[0] == TOKEN_KEYWORD_PROMPT)
  ||  (main_token_types[0] == TOKEN_KEYWORD_QUIT)
  ||  (main_token_types[0] == TOKEN_KEYWORD_REHASH)
  ||  (main_token_types[0] == TOKEN_KEYWORD_SOURCE)
  ||  (main_token_types[0] == TOKEN_KEYWORD_STATUS)
  ||  (main_token_types[0] == TOKEN_KEYWORD_SYSTEM)
  ||  (main_token_types[0] == TOKEN_KEYWORD_TEE)
  ||  (main_token_types[0] == TOKEN_KEYWORD_USE)
  ||  (main_token_types[0] == TOKEN_KEYWORD_WARNINGS))
  {
    main_statement_type= main_token_types[0];
  }
  else
  {
    main_statement_type= 0;
  }

  /*
    The special cases of "\q" etc.:
    If token[0] is "\" and token[q] == "q" (lower case only), that's equivalent to "quit".
    They're still separate tokens -- changing tokenize() to say "\=" is not a keyword is not
    appropriate because "\" really should be an operator. But treat both tokens as the
    same keyword, TOKEN_KEYWORD_QUIT.
    This is for mysql compatibility.
    I couldn't find a consistent rule, but it seems that the statement pair could be at the
    start, could be at the end, or could be at the end before a delimiter.
    The list is in order by TOKEN_KEYWORD_... value.
  */
  int xx= -1;
  if (i2  >= 2)
  {
    /* Todo: what about the delimiter? */
    if ((main_token_offsets[1] == 1) && (main_token_lengths[1] == 1)) xx= 0;
    if ((main_token_lengths[main_token_count - 2] == 1) && (main_token_lengths[main_token_count - 1] == 1)) xx= main_token_count - 2;
    }
  if (i2 >= 3)
  {
    s= text.mid(main_token_offsets[i2 - 1], main_token_lengths[i2 - 1]);
    /* Todo: compare with delimiter, which isn't always semicolon. */
    if (s == (QString)ocelot_delimiter_str)
    {
      if ((main_token_lengths[main_token_count - 3] == 1) && (main_token_lengths[main_token_count - 2] == 1)) xx= main_token_count - 3;
    }
  }
  if (xx >= 0)
  {
    s= text.mid(main_token_offsets[xx], 2);
    if (s == QString("\\?")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_QUESTIONMARK;
    if (s == QString("\\C")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_CHARSET;
    if (s == QString("\\c")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_CLEAR;
    if (s == QString("\\r")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_CONNECT;
    if (s == QString("\\d")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_DELIMITER;
    if (s == QString("\\e")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_EDIT;
    if (s == QString("\\G")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_EGO;
    if (s == QString("\\g")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_GO;
    if (s == QString("\\h")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_HELP;
    if (s == QString("\\n")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_NOPAGER;
    if (s == QString("\\t")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_NOTEE;
    if (s == QString("\\w")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_NOWARNING;
    if (s == QString("\\P")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_PAGER;
    if (s == QString("\\p")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_PRINT;
    if (s == QString("\\R")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_PROMPT;
    if (s == QString("\\q")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_QUIT;
    if (s == QString("\\#")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_REHASH;
    if (s == QString("\\.")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_SOURCE;
    if (s == QString("\\s")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_STATUS;
    if (s == QString("\\!")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_SYSTEM;
    if (s == QString("\\T")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_TEE;
    if (s == QString("\\u")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_USE;
    if (s == QString("\\W")) main_statement_type= main_token_types[xx]= main_token_types[xx + 1]= TOKEN_KEYWORD_WARNINGS;
  }
}


/*
  Find start of body in CREATE ... FUNCTION|PROCEDURE|TRIGGER statement
  This is tricky and might need revisiting if syntax changes in later MySQL versions.
  First find out whether the statement is CREATE ... FUNCTION|PROCEDURE|TRIGGER. A definer clause might have to be skipped.
  Then if it's CREATE TRIGGER, it's reasonably easy: just skip till past FOR EACH ROW.
  If it's CREATE PROCEDURE|FUNCTION:
    skip past ()s
   (Function) Skip returns clause i.e. skip until anything is seen which is not one of the following:
      anything within ()s
      ASCII BIGINT BINARY BIT BLOB BOOL BOOLEAN CHAR CHARACTER CHARSET COLLATE DATE DATETIME DEC
      DECIMAL DOUBLE ENUM FLOAT
      GEOMETRY GEOMETRYCOLLECTION INT INTEGER LINESTRING LONGBLOB LONGTEXT MEDIUMBLOB MEDIUMINT MEDIUMTEXT
      MULTILINESTRING MULTIPOINT MULTIPOLYGON NUMERIC POINT POLYGON PRECISION RETURNS
      SERIAL SET(...) SMALLINT TIME TIMESTAMP TINYBLOB TINYINT TINYTEXT UNICODE UNSIGNED VARCHAR VARYING YEAR ZEROFILL
      name if previous is CHARSET | CHARACTER SET | COLLATE
   (Function or Procedure) Skip words that are in characteristics i.e.
     COMMENT 'string', LANGUAGE SQL, DETERMINISTIC, NOT DETERMINISTIC,
     CONTAINS SQL, NO SQL, READS SQL DATA, MODIFIES SQL DATA,
     SQL SECURITY DEFINER, SQL SECURITY INVOKER
   Skipping comments too
   Return: offset for first word of body, or -1 if not-create-routine | body-not-found
*/
int MainWindow::find_start_of_body(QString text, int *i_of_function)
{
  int i;
  int create_seen= 0;
  int procedure_seen= 0;
  int function_seen= 0;
  int trigger_seen= 0;
  int trigger_for_seen= 0;
  int trigger_row_seen= 0;
  int parameter_list_seen= 0;
  int parentheses_count= 0;
  int characteristic_seen= 0;
  int data_type_seen= 0;
  int character_set_seen= 0;
  int collate_seen= 0;

  for (i= 0; main_token_lengths[i] != 0; ++i)
  {
    if ((main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_SLASH)
     || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_MINUS))
    {
      continue;
    }
    if (trigger_row_seen == 1)
    {
      return i;
    }
    if (main_token_types[i] == TOKEN_KEYWORD_CREATE)
    {
      create_seen= 1;
      continue;
    }
    if (create_seen != 1)
    {
      return -1;
    }
    if (main_token_types[i] == TOKEN_KEYWORD_PROCEDURE)
    {
      procedure_seen= 1;
      continue;
    }
    if (main_token_types[i] == TOKEN_KEYWORD_FUNCTION)
    {
      function_seen= 1;
      *i_of_function= i;
      continue;
    }
    if (main_token_types[i] == TOKEN_KEYWORD_TRIGGER)
    {
      trigger_seen= 1;
      continue;
    }
    if (trigger_seen == 1)
    {
      if (main_token_types[i] == TOKEN_KEYWORD_FOR) trigger_for_seen= 1;
      if (trigger_for_seen == 1)
      {
        if (main_token_types[i] == TOKEN_KEYWORD_ROW) trigger_row_seen= 1;
      }
    }
    if ((function_seen == 1) || (procedure_seen == 1))
    {
      if ((parameter_list_seen == 0) && (main_token_types[i] == TOKEN_TYPE_OPERATOR) && (main_token_lengths[i] == 1))
      {
        if (text.mid(main_token_offsets[i], main_token_lengths[i]) == "(")
        {
          ++parentheses_count;
          continue;
        }
        if (text.mid(main_token_offsets[i], main_token_lengths[i]) == ")")
        {
          --parentheses_count;
          if (parentheses_count == 0)
          {
            parameter_list_seen= 1;
            continue;
          }
        }
      }
    }

    if ((function_seen == 1) && (parameter_list_seen == 1) && (characteristic_seen == 0))
    {
      if (text.mid(main_token_offsets[i], main_token_lengths[i]) == "(")
      {
        ++parentheses_count;
        continue;
      }
      if (text.mid(main_token_offsets[i], main_token_lengths[i]) == ")")
      {
        --parentheses_count;
        continue;
      }
      if (parentheses_count != 0)
      {
        continue;
      }
      if (main_token_types[i] == TOKEN_KEYWORD_RETURNS)
      {
        continue;
      }
      if (data_type_seen == 0)
      {
        if ((main_token_types[i] == TOKEN_KEYWORD_ASCII)
         || (main_token_types[i] == TOKEN_KEYWORD_BIGINT)
         || (main_token_types[i] == TOKEN_KEYWORD_BINARY)
         || (main_token_types[i] == TOKEN_KEYWORD_BIT)
         || (main_token_types[i] == TOKEN_KEYWORD_BLOB)
         || (main_token_types[i] == TOKEN_KEYWORD_BOOL)
         || (main_token_types[i] == TOKEN_KEYWORD_BOOLEAN)
         || (main_token_types[i] == TOKEN_KEYWORD_CHAR)
         || (main_token_types[i] == TOKEN_KEYWORD_CHARACTER)
         || (main_token_types[i] == TOKEN_KEYWORD_DATE)
         || (main_token_types[i] == TOKEN_KEYWORD_DATETIME)
         || (main_token_types[i] == TOKEN_KEYWORD_DEC)
         || (main_token_types[i] == TOKEN_KEYWORD_DECIMAL)
         || (main_token_types[i] == TOKEN_KEYWORD_DOUBLE)
         || (main_token_types[i] == TOKEN_KEYWORD_ENUM)
         || (main_token_types[i] == TOKEN_KEYWORD_FLOAT)
         || (main_token_types[i] == TOKEN_KEYWORD_GEOMETRY)
         || (main_token_types[i] == TOKEN_KEYWORD_GEOMETRYCOLLECTION)
         || (main_token_types[i] == TOKEN_KEYWORD_INT)
         || (main_token_types[i] == TOKEN_KEYWORD_INTEGER)
         || (main_token_types[i] == TOKEN_KEYWORD_LINESTRING)
         || (main_token_types[i] == TOKEN_KEYWORD_LONGBLOB)
         || (main_token_types[i] == TOKEN_KEYWORD_LONGTEXT)
         || (main_token_types[i] == TOKEN_KEYWORD_MEDIUMBLOB)
         || (main_token_types[i] == TOKEN_KEYWORD_MEDIUMINT)
         || (main_token_types[i] == TOKEN_KEYWORD_MEDIUMTEXT)
         || (main_token_types[i] == TOKEN_KEYWORD_MULTILINESTRING)
         || (main_token_types[i] == TOKEN_KEYWORD_MULTIPOINT)
         || (main_token_types[i] == TOKEN_KEYWORD_MULTIPOLYGON)
         || (main_token_types[i] == TOKEN_KEYWORD_NUMERIC)
         || (main_token_types[i] == TOKEN_KEYWORD_POINT)
         || (main_token_types[i] == TOKEN_KEYWORD_POLYGON)
         || (main_token_types[i] == TOKEN_KEYWORD_SERIAL)
         || (main_token_types[i] == TOKEN_KEYWORD_SET)
         || (main_token_types[i] == TOKEN_KEYWORD_SMALLINT)
         || (main_token_types[i] == TOKEN_KEYWORD_TIME)
         || (main_token_types[i] == TOKEN_KEYWORD_TIMESTAMP)
         || (main_token_types[i] == TOKEN_KEYWORD_TINYBLOB)
         || (main_token_types[i] == TOKEN_KEYWORD_TINYINT)
         || (main_token_types[i] == TOKEN_KEYWORD_TINYTEXT)
         || (main_token_types[i] == TOKEN_KEYWORD_UNICODE)
         || (main_token_types[i] == TOKEN_KEYWORD_VARCHAR)
         || (main_token_types[i] == TOKEN_KEYWORD_YEAR))
        {
          data_type_seen= 1;
          continue;
        }
      }
      if (data_type_seen == 1)
      {
        if ((main_token_types[i] == TOKEN_KEYWORD_PRECISION)
         || (main_token_types[i] == TOKEN_KEYWORD_VARYING)
         || (main_token_types[i] == TOKEN_KEYWORD_UNSIGNED)
         || (main_token_types[i] == TOKEN_KEYWORD_ZEROFILL))
        {
          continue;
        }
        if ((main_token_types[i] == TOKEN_KEYWORD_CHARSET) || (main_token_types[i] == TOKEN_KEYWORD_CHARACTER))
        {
          character_set_seen= 1;
          continue;
        }
        if (main_token_types[i] == TOKEN_KEYWORD_COLLATE)
        {
          collate_seen= 1;
          continue;
        }
        if (character_set_seen == 1)
        {
          if (main_token_types[i] == TOKEN_KEYWORD_SET)
          {
            continue;
          }
          character_set_seen= 0;
          continue;
        }
        if (collate_seen == 1)
        {
          collate_seen= 0;
          continue;
        }
      }
    }

    if (((function_seen == 1) || (procedure_seen == 1)) && (parameter_list_seen == 1))
    {
      if ((text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "COMMENT")
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "LANGUAGE")
       || (main_token_types[i] == TOKEN_KEYWORD_SQL)
       || (main_token_types[i] == TOKEN_KEYWORD_DETERMINISTIC)
       || (main_token_types[i] == TOKEN_KEYWORD_NOT)
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "CONTAINS")
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "NO")
       || (main_token_types[i] == TOKEN_KEYWORD_READS)
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "DATA")
       || (main_token_types[i] == TOKEN_KEYWORD_MODIFIES)
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "SECURITY")
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "DEFINER")
       || (text.mid(main_token_offsets[i], main_token_lengths[i]).toUpper() == "INVOKER")
       || (main_token_types[i] == TOKEN_TYPE_LITERAL_WITH_SINGLE_QUOTE)
       || (main_token_types[i] == TOKEN_TYPE_LITERAL_WITH_DOUBLE_QUOTE))
      {
        characteristic_seen= 1;
        continue;
      }
    }
    if ((main_token_types[i] == TOKEN_KEYWORD_DATABASE)
     || (main_token_types[i] == TOKEN_KEYWORD_EVENT)
     || (main_token_types[i] == TOKEN_KEYWORD_INDEX)
     || (main_token_types[i] == TOKEN_KEYWORD_LOGFILE)
     || (main_token_types[i] == TOKEN_KEYWORD_SERVER)
     || (main_token_types[i] == TOKEN_KEYWORD_TABLE)
     || (main_token_types[i] == TOKEN_KEYWORD_TABLESPACE)
     || (main_token_types[i] == TOKEN_KEYWORD_VIEW))
    {
      if (function_seen + procedure_seen + trigger_seen == 0)
      {
        return -1;
      }
    }
    if ((function_seen == 1) || (procedure_seen == 1))
    {
      if (parameter_list_seen == 0) continue;
      return i;
    }
  }
  return -1;
}


/*
  The special cases of FUNCTION, ROW, DO, END, BEGIN. We don't want to mark them as special unless
  we know they're really serving a non-identifier role in the statment, but they might be,
  because they're not reserved. This has to be right because later we might count BEGINs
  minus ENDs in order to figure out whether a compound statement has ended.
  FUNCTION is a keyword if: previous == DROP or CREATE [definer=...] or DROP (this is known in advance)
  ROW is a keyword if: previous == EACH
  DO is a keyword if: last statement-beginner keyword was WHILE
                      and next is a statement-beginner keyword or a label
                      (but actually this just checks whether we're within WHILE -- Todo: must fix that someday)
  END is a keyword if: previous == ; or BEGIN
  BEGIN is a keyword if: previous ==  ; or : or BEGIN or DO or ELSE or LOOP or ROW or THEN
                         or previous = ) and word before matching ( is PROCEDURE or FUNCTION or TRIGGER
                         and next is not :
                         or it's first word in main body or handler body, which is what causes most of the difficulties.
*/
void MainWindow::tokens_to_keywords_revert(int i_of_body, int i_of_function, QString text)
{
  int i;                                                /* index of current token */
  int i_prev= -1;                                       /* index of last non-comment token */
  int i_of_while= -1;                                   /* index of last keyword = while */

  for (i= 0; main_token_lengths[i] != 0; ++i)
  {
    /* If this is a comment then don't note its position i.e. i_prev is only for non-comment tokens */
    if ((main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
     || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_SLASH)
     || (main_token_types[i] == TOKEN_TYPE_COMMENT_WITH_MINUS))
    {
      continue;
    }
    if (i_prev != -1)
    {
      /* If this is ":" then the last item can't have been a keyword */
      if (QString::compare(text.mid(main_token_offsets[i], main_token_lengths[i]), ":") == 0)
      {
        if ((main_token_types[i_prev] >= TOKEN_KEYWORDS_START))
        {
          main_token_types[i_prev]= TOKEN_TYPE_OTHER;
        }
      }
      /* If this is FUNCTION then it's not aa keyword unless previous was DROP or CREATE [definer=...] */
      if (main_token_types[i] == TOKEN_KEYWORD_FUNCTION)
      {
        if ((i == i_of_function) || (main_token_types[i_prev] == TOKEN_KEYWORD_DROP)) ;
        else main_token_types[i]= TOKEN_TYPE_OTHER;
      }
      /* If this is ROW then it's not a keyword unless previous was EACH as in FOR EACH ROW */
      if (main_token_types[i] == TOKEN_KEYWORD_ROW)
      {
        if (main_token_types[i_prev] == TOKEN_KEYWORD_EACH) ;
        else main_token_types[i]= TOKEN_TYPE_OTHER;
      }
      /* If this is DO then it's not a keyword unless it's after WHILE (condition) */
      if (main_token_types[i] == TOKEN_KEYWORD_DO)
      {
        if (i_of_while != -1) i_of_while= -1;
        else main_token_types[i]= TOKEN_TYPE_OTHER;
      }
      /* If this is END then it's not a keyword unless previous waa ; or BEGIN */
      if (main_token_types[i] == TOKEN_KEYWORD_END)
      {
        if ((QString::compare(text.mid(main_token_offsets[i_prev], main_token_lengths[i_prev]), ";") == 0)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_BEGIN)) ;
        else main_token_types[i]= TOKEN_TYPE_OTHER;
      }
      /* If this is BEGIN then it's not a keyword unless it's body start or prev = BEGIN|DO|ELSE|LOOP|ROW|THEN */
      if (main_token_types[i] == TOKEN_KEYWORD_BEGIN)
      {
        if ((i == i_of_body)
        || (QString::compare(text.mid(main_token_offsets[i_prev], main_token_lengths[i_prev]), ":") == 0)
        || (QString::compare(text.mid(main_token_offsets[i_prev], main_token_lengths[i_prev]), ";") == 0)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_BEGIN)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_DO)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_ELSE)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_LOOP)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_ROW)
        || (main_token_types[i_prev] == TOKEN_KEYWORD_THEN)) ;
        else main_token_types[i]= TOKEN_TYPE_OTHER;
      }
    }
    if (main_token_types[i] == TOKEN_KEYWORD_WHILE) i_of_while= i;
    if ((QString::compare(text.mid(main_token_offsets[i], main_token_lengths[i]), "HANDLER", Qt::CaseInsensitive) == 0)
     && (main_token_types[next_token(i)] == TOKEN_KEYWORD_FOR))
    {
      /* DECLARE ... HANDLER FOR might be followed by BEGIN so we have to find where statement starts */
      int i_plus_1, i_plus_2, i_plus_3, i_plus_4, i_next;
      i= next_token(i);
      for (;;)
      {
        i_plus_1= next_token(i);
        i_plus_2= next_token(i_plus_1);
        i_plus_3= next_token(i_plus_2);
        i_plus_4= next_token(i_plus_3);
        if ((main_token_types[i_plus_1] == TOKEN_KEYWORD_SQLSTATE)
         && (QString::compare(text.mid(main_token_offsets[i_plus_2], main_token_lengths[i_plus_2]), "VALUE", Qt::CaseInsensitive) == 0))
        {
          if (main_token_types[i_plus_3] >= TOKEN_KEYWORDS_START) main_token_types[i_plus_3]= TOKEN_TYPE_OTHER;
          i_next= i_plus_4;
        }
        else if (main_token_types[i_plus_1] == TOKEN_KEYWORD_SQLSTATE)
        {
          if (main_token_types[i_plus_2] >= TOKEN_KEYWORDS_START) main_token_types[i_plus_2]= TOKEN_TYPE_OTHER;
          i_next= i_plus_3;
        }
        else if ((main_token_types[i_plus_1] == TOKEN_KEYWORD_NOT)
         && (QString::compare(text.mid(main_token_offsets[i_plus_2], main_token_lengths[i_plus_2]), "FOUND", Qt::CaseInsensitive) == 0))
        {
          i_next= i_plus_3;
        }
        else
        {
          if (main_token_types[i_plus_1] >= TOKEN_KEYWORDS_START) main_token_types[i_plus_1]= TOKEN_TYPE_OTHER;
          i_next= i_plus_2;
        }
        if (QString::compare(text.mid(main_token_offsets[i_next], main_token_lengths[i_next]), ",") == 0)
        {
          i= i_next;
          continue;
        }
        break;
      }
      /* now i_next == first_word_in_statement in DECLARE HANDLER, unless syntax is bad, I hope */
      i= i_next;
    }
    i_prev= i;
  }
}

/* returns next token after i, skipping comments, but do not go past end */
int MainWindow::next_token(int i)
{
  int i2;

  for (i2= i;;)
  {
    if (main_token_lengths[i2] == 0) break;
    ++i2;
    if ((main_token_types[i2] == TOKEN_TYPE_COMMENT_WITH_OCTOTHORPE)
    || (main_token_types[i2] == TOKEN_TYPE_COMMENT_WITH_SLASH)
    || (main_token_types[i2] == TOKEN_TYPE_COMMENT_WITH_MINUS))
    {
      continue;
    }
    break;
  }
  return i2;
}

/*
  Todo: disconnect old if already connected.
  TODO: LOTS OF ERROR CHECKS NEEDED IN THIS!
*/
int MainWindow::connect_mysql(unsigned int connection_number)
{

  mysql_init(&mysql[connection_number]); /* Todo: avoid repetition, this is init'd elsewhere. */

  if (the_connect(connection_number))
  {
    put_diagnostics_in_result();
    statement_edit_widget->result.append(tr("Failed to connect. Use menu item File|Connect to try again"));
    return 1;
  }
  statement_edit_widget->result= tr("OK");

  /*
    Todo: This overrides any earlier PROMPT statements by user.
    Probably what we want is a flag: "if user did PROMPT, don't override it."
    Or we want PROMPT statement to change ocelot_prompt.
  */
  statement_edit_widget->prompt_default= ocelot_prompt;
  statement_edit_widget->prompt_as_input_by_user= statement_edit_widget->prompt_default;

  /*
    Collect some variables in case they're needed for "prompt".
    Todo: check for errors after these mysql_ calls.
    Not using the mysql_res global, since this is not for user to see.
  */
  mysql_query(&mysql[connection_number], "select version(), database(), @@port, current_user(), connection_id()");
  MYSQL_RES *mysql_res_for_connect;
  MYSQL_ROW connect_row;
  QString s;
  int i;

  // unsigned long connect_lengths[1];
  mysql_res_for_connect= mysql_store_result(&mysql[connection_number]);
  connect_row= mysql_fetch_row(mysql_res_for_connect);

  /* lengths= mysql_fetch_lengths(mysql_res_for_connect); */
  statement_edit_widget->dbms_version= connect_row[0];
  statement_edit_widget->dbms_database= connect_row[1];
  statement_edit_widget->dbms_port= connect_row[2];
  s= connect_row[3];
  statement_edit_widget->dbms_current_user= s;
  i= s.indexOf(QRegExp("@"), 0);
  if (i > 0) s= s.left(i);
  statement_edit_widget->dbms_current_user_without_host= s;
  statement_edit_widget->dbms_connection_id=atoi(connect_row[4]);
  s= mysql_get_host_info(&mysql[connection_number]);
  i= s.indexOf(QRegExp(" "), 0);
  if (i > 0) s= s.left(i);
  statement_edit_widget->dbms_host= s;
  mysql_free_result(mysql_res_for_connect);
  is_mysql_connected= 1;
  return 0;
}


/*
  For copyright and license notice of CodeEditor function, see beginning of this program.

  CodeEditor is a widget that inherits QPlainTextEdit.
     Therefore it inherits from QAbstractScrollArea, which has viewports.
  prompt_widget is a separate widget within CodeEditor.
      Paint the prompt on prompt_widget.
      Place prompt_widget over CodeEditor's viewport's left margin area.
      Doubtless you'll want to handle mouse events on prompt_widget, for breakpoints.
  Need to adjust when number of lines changes, when viewport is scrolled, or editor's size changes.

  In MainWindow:: we refer to components with "statement_edit_widget->...".
  In CodeEditor:: we refer to components with "this->...". They're the same things.

  Re Prompts ...
    Mainly we follow what mysql client does with prompt, as described in
    http://dev.mysql.com/doc/refman/5.6/en/mysql-commands.html.
    But the following are additional:
    \N                  (todo)                             seems to be "name of server" e.g. mysql or MariaDB
    \2                  Indicates the rest of the prompt gets repeated on all statement lines. Essential if \L is used.
    \L                  Statement line number
    \K(left-prompt)     See comment before prompt_translate_k()
    For lines 2ff, the prompt becomes > unless there was something after \L, and is right-justified.
    Bug: for "prompt \w", the \w will be taken as a "nowarnings" keyword, because it's the last thing on the line.
    Todo: There are several prompts that require asking the server. The probable way to handle them is:
          At connect time:
          mysql_get_host_info() to get the host
          select current_user(), version(), database(), @@port to get other server stuff
*/

/* Following should be multi-occurrence and should be part of CodeEditor */
/* Todo: Sizes should be dynamic! You'll only see stuff for the first 10 lines with what you've got here! */
#define K_SIZE 10
int k_line_number[K_SIZE];
QColor k_color[K_SIZE];
QString k_string[K_SIZE];

CodeEditor::CodeEditor(QWidget *parent) : QPlainTextEdit(parent)
{
  prompt_widget= new prompt_class(this);

  connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(update_prompt_width(int)));
  connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(update_prompt(QRect,int)));
  connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));
  /*
    It is necessary to calculate prompt area width and highlight the first line
    when the editor is first created.
  */
  /* I decided "update_prompt_width(0);" here would be unnecessary since I redo prompt width after making a default etc. */
  highlightCurrentLine();
}


/* Todo: translate, i.e. convert \\ to \, convert \linenumber to (binary) 0x00, etc. */
/* On lines 2ff, the prompt is merely ">" unless there was a preceding "\2" */
QString CodeEditor::prompt_translate(int line_number_x)
{
  QString s, s_char, s_out;
  int is_2_seen= 0;
  time_t basetime;
  struct tm * timeinfo;
  char formatted_time[80];
  int k_index;

  for (k_index= 0; k_index < K_SIZE; ++k_index) k_line_number[k_index]= -1;
  /*
    Todo: Getting date and time is slow.
    You should find out in advance (when user changes the prompt) whether you really will need it.
    Or you should calculate it only when you see \D etc., but that might cause apparent inconsistencies.
    Or you could check "Has the time already been calculated?"
  */
  time(&basetime);
  timeinfo= localtime (&basetime);

  s= this->prompt_as_input_by_user;
  s_out= (QString)"";
  for (int i= 0; i < s.size(); ++i)
  {
    s_char= s.mid(i, 1);
    if ((s_char == "\\") && (i < s.size() - 1))
    {
      ++i;
      s_char= s.mid(i, 1);
      if (s_char == "2")        /* \2 is to indicate that following items come on lines 2ff, not just line 1. Ocelot-only. */
      {
        s_char= (QString)""; is_2_seen= 1;
      }
      if (s_char == "c")        /* \c is for statement counter. We keep it in statement_edit_widget, not main window -- maybe that's bad. */
      {
        s_char= QString::number(statement_count);
      }
      if (s_char == "D")        /* \D is for full current date, e.g. "Fri May 23 14:26:18 2014" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%a %b %e %H:%M:%S %Y", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "d")        /* \d is for database. Discovered at connect time. Changed if user says "use". */
      {
        s_char= dbms_database;
      }
      if (s_char == "h")        /* \h is for host. */
      {
        s_char= dbms_host;
      }
      if (s_char == "K")
      {
        int k_result= prompt_translate_k(s, i);
        if (k_result != 0)
        {
          i+= k_result;
          continue;
        }
      }
      if (s_char == "L")        /* \L is for line number. Ocelot-only. Most effective if \2 seen. */
      {
        s_char= QString::number(line_number_x);
      }
      if (s_char == "l")        /* \l is for delimiter. */
      {
        s_char= delimiter;
      }
      if (s_char == "m")        /* \m is for minutes of the current time, e.g. "18" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%M", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "n")        /* \n is for newline. It goes in the stream but there's no apparent effect. */
                                /* todo: try harder. use markup instead of \n? crlf? */
      {
        s_char= "\n";
      }
      if (s_char == "O")        /* \O is for current month in 3-letter format, e.g. "Feb" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%b", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "o")        /* \o is for current month in numeric format, e.g. "12" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%m", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "P")        /* \P is for am/pm, e.g. "PM" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%p", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "p") {        /* \p is for port */
        s_char= dbms_port;
      }
      if (s_char == "R")        /* \R is for current hour on a 24-hour clock, e.g. "19" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%H", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "r")        /* \r is for current hour on a 12-hour clock, e.g. "11" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%r", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "S")        /* \S is for semicolon i.e. ";" */
      {
        s_char= ";";
      }
      if (s_char == "s")        /* \s is for seconds of the current time, e.g. "58" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%S", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "t")        /* \t is for tab. Appearance can be inconsistent. */
      {
        s_char= "\t";
      }
      if (s_char == "U")        /* \U is for user@host. */
      {
        s_char= dbms_current_user;
      }
      if (s_char == "u")        /* \u is for user. */
      {
        s_char= dbms_current_user_without_host;
      }
      if (s_char == "v")        /* \v is for server version. Discovered at connect time. */
      {
        s_char= dbms_version;
      }
      if (s_char == "w")        /* \w is for current day of the week in three-letter format, e.g. "Mon" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%a", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "Y")        /* \Y is for current year in four-digit format, e.g. "2014" */
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%Y", timeinfo);
        s_char= formatted_time;
      }
      if (s_char == "y")
      {
        strftime(formatted_time, sizeof(formatted_time) - 1, "%y", timeinfo);
        s_char= formatted_time;
      }
    }
    if ((line_number_x < 2) || (is_2_seen == 1))
    {
      s_out.append(s_char);
    }
  }
  if ((line_number_x > 1) && (is_2_seen == 0)) s_out.append((QString)">");
  return (s_out);
}

/*
  prompt_translate_k() is called from prompt_translate() when \K is seen.
  The prompt string may contain "\K(line-number,color-name,string)" -- no spaces after K.
  which will put out a string, at the specified line, with the specified color.
  The string will be left-justified so it won't interfere with the rest of the prompt.
  If you want a space between the left and right justification, put ' ' at end of string
  The string cannot contain ) or ' or ", there are no escapes.
  A line number may not be specified twice.
  Erroneous input is ignored.
  Example: '\K(5,red,B)' means: on line 5, left justified, put 'B'.
  Width will increase if and only if the line number exists.
  This is intended for use with debugger breakpoints, but could be used for statements.
  Todo: find out why this is called a large number of times
  Todo: really, you should be using the usual parser, not this quickie hack job
*/
int CodeEditor::prompt_translate_k(QString s, int i)
{
  QString s_char;
  QString string_for_line_number;
  QString string_for_color;
  QString string_for_string;
  int i_left_parenthesis, i_first_comma, i_second_comma, i_right_parenthesis;
  int i2;

  i_left_parenthesis= i_first_comma= i_second_comma= i_right_parenthesis= -1;
  for (i2= i; i2 < s.size(); ++i2)
  {
    s_char= s.mid(i2, 1);
    if ((s_char == "(") && (i_left_parenthesis == -1)) i_left_parenthesis= i2;
    if ((s_char == ",") && (i_first_comma == -1)) i_first_comma= i2;
    else if ((s_char == ",") && (i_first_comma != -1) && (i_second_comma == -1)) i_second_comma= i2;
    if ((s_char == ")") && (i_second_comma != -1) && (i_right_parenthesis == -1)) i_right_parenthesis= i2;
  }

  if (i_left_parenthesis != i + 1) return 0; /* bad left parenthesis */
  if (i_first_comma == -1) return 0; /* bad first comma */
  if (i_second_comma == -1) return 0; /* bad second comma */
  if (i_right_parenthesis == -1) return 0; /* bad right parenthesis */

  string_for_line_number= s.mid(i_left_parenthesis + 1, (i_first_comma - i_left_parenthesis) - 1);
  string_for_color= s.mid(i_first_comma + 1, (i_second_comma - i_first_comma) - 1);
  string_for_string= s.mid(i_second_comma + 1, (i_right_parenthesis - i_second_comma) - 1);

  bool ok;
  int line_number;
  line_number= string_for_line_number.toInt(&ok, 10);
  if ((ok == false) || (line_number <= 0)) return 0; /* bad line number */

  QColor color;
  color.setNamedColor(string_for_color);
  if (color.isValid() == false) return 0; /* bad color */

  /* Find an unused point in k_line_number[], set it to line number. If all points are full, too bad. (Will fix soon!) */
  for (int k_index= 0; k_index < K_SIZE; ++k_index)
  {
    if (k_line_number[k_index] == -1)
    {
      k_line_number[k_index]= line_number;
      k_color[k_index]= color;
      k_string[k_index]= string_for_string;
      break;
    }
  }

  return (i_right_parenthesis - i) + 1;
}


/*
  Set left margin of viewport to the size we need to draw the prompt.
   This, presumably, is what prompt_width_calculate returns.
   Take the number of digits in the last line, multiply that by maximum width of a digit.
   We don't know in advance whether which is wider, the first line or the last line.
   (It's calculable in advance, but I didn't bother.)
   So we calculate both, and take the wider one, even if line 1 might not be visible.
   Todo: It's possible that width("11") is less than width("2"), so counting digits would be sensible.
*/
int CodeEditor::prompt_width_calculate()
{
  int line_width_first, line_width_last;
  QString sq;
  /* The new style. Ignoring all the above, give us width. */
  /* Todo: following should be a real interpretation. */
  /* this->prompt_current= this->prompt_translate(blockCount() + 1); */
  int prompt_width= 3;

  sq= prompt_translate(1);
  line_width_first= fontMetrics().width(sq);
  sq= prompt_translate(blockCount() + 1);
  line_width_last= fontMetrics().width(sq);
  if (line_width_first < line_width_last) prompt_width= line_width_last;
  else prompt_width= line_width_first;
#ifdef DEBUGGER
  /* TEST */
  prompt_width+= fontMetrics().width("B ");
#endif
  return prompt_width;

  /* Todo: check why this doesn't work -- prompt_width+= fontMetrics().width(this->prompt_current); */
  /* QString ss= prompt_translate(xxx); */
  QString ss;
  for (int i= 0; ss[i] != 0; ++i) prompt_width+= fontMetrics().width(ss[i]);
  return prompt_width;
}


/*
  We get here because of:
  "connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(update_prompt_width(int)));"
*/
void CodeEditor::update_prompt_width(int /* newBlockCount */)
{
  setViewportMargins(prompt_width_calculate(), 0, 0, 0);
}


/*
  We get here because of:
  "connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(update_prompt(QRect,int)));"
  When we update the width of the line number area, we simply call QAbstractScrollArea::setViewportMargins().
*/
void CodeEditor::update_prompt(const QRect &rect, int dy)
{
  if (dy)
    prompt_widget->scroll(0, dy);
  else
    prompt_widget->update(0, rect.y(), prompt_widget->width(), rect.height());
  if (rect.contains(viewport()->rect()))
    update_prompt_width(0);
}


/*
  This slot is invoked when the editor's viewport has been scrolled.
  The QRect given as argument is the part of the editing area that is to be updated (redrawn).
  dy holds the number of pixels the view has been scrolled vertically.
  When the size of the editor changes, we also need to resize the line number area.
*/
void CodeEditor::resizeEvent(QResizeEvent *e)
{
  QPlainTextEdit::resizeEvent(e);

  QRect cr= contentsRect();
  prompt_widget->setGeometry(QRect(cr.left(), cr.top(), prompt_width_calculate(), cr.height()));
}


/*
  We get to highlightCurrentline because of earlier
  "connect(this, SIGNAL(cursorPositionChanged()), this, SLOT(highlightCurrentLine()));"
  Todo: the hard coding Qt::yellow should instead be something chosen for action_statement().
  I made one change from the original code:
    Originally there was a } brace after "extraSelections.append(selection);".
    I moved it so there is a } brace after "setExtraSelections(extraSelections);".
    I believe this should only affect debug_widget[], which is read-only except when debug_highlight_line() happens.
*/
void CodeEditor::highlightCurrentLine()
{
  QList<QTextEdit::ExtraSelection> extraSelections;

  if (!isReadOnly())
  {
    QTextEdit::ExtraSelection selection;

    QColor lineColor= QColor(Qt::yellow).lighter(160);

    selection.format.setBackground(lineColor);
    selection.format.setProperty(QTextFormat::FullWidthSelection, true);
    selection.cursor= textCursor();
#ifdef DEBUGGER
    block_number= selection.cursor.blockNumber();                           /* effectively = line number base 0 */
#endif
    selection.cursor.clearSelection();
    extraSelections.append(selection);
  }
  setExtraSelections(extraSelections);
}


/* The prompt_widget_paintevent() is called from prompt_widget whenever it receives a paint event. */
void CodeEditor::prompt_widget_paintevent(QPaintEvent *event)
{
  QColor bgcolor;
  QColor textcolor;
  QString prompt_text;

  QPainter painter(prompt_widget);
  bgcolor= this->statement_edit_widget_left_bgcolor;
  painter.fillRect(event->rect(), bgcolor);

  QTextBlock block= firstVisibleBlock();
  int blockNumber= block.blockNumber();

  /* At tone time there was an overflow check here: "if (blockNumber > 1000) ... exit(1);". I couldn't remember why. */

  /* We start off by painting the widget's background. */
  int top= (int) blockBoundingGeometry(block).translated(contentOffset()).top();
  int bottom= top + (int) blockBoundingRect(block).height();

  /*
    We will now loop through all visible lines and paint the prompt in the extra area for each line.
    Notice that in a plain text edit each line will consist of one QTextBlock;
    though, if line wrapping is enabled, a line may span several rows in the text edit's viewport.
  */

  /*
    We get the top and bottom y-coordinate of the first text block,
    and adjust these values by the height of the current text block in each iteration in the loop.
    Notice that we check if the block is visible in addition to check if it is in the areas viewport -
    a block can, for example, be hidden by a window placed over the text edit.
  */
  while (block.isValid() && top <= event->rect().bottom())
  {
    if (block.isVisible() && bottom >= event->rect().top())
    {
      textcolor= this->statement_edit_widget_left_treatment1_textcolor;
      prompt_text= prompt_translate(blockNumber + 1);
      QString s= prompt_text;
      /* QString number= QString::number(blockNumber + 1); */
#ifdef DEBUGGER
      /* hmm, actually this loop could stop when k_line_number[k_index] == -1 too */
      for (int k_index= 0; k_index < K_SIZE; ++k_index)
      {
        if (blockNumber == k_line_number[k_index] - 1)
        {
          painter.setPen(QColor(k_color[k_index]));
          painter.drawText(0, top, 10, fontMetrics().height(), Qt::AlignLeft, k_string[k_index]);
          break;
        }
      }
#endif
      painter.setPen(textcolor);
      painter.drawText(0, top, prompt_widget->width(), fontMetrics().height(),
                       Qt::AlignRight, s);
    }
    block= block.next();
    top= bottom;
    bottom= top + (int) blockBoundingRect(block).height();
    ++blockNumber;
  }
}

/* End of CodeEditor. End of effect of licence of CodeEditor. */

/*
  TextEditFrame
  This is one of the components of result_grid
*/
TextEditFrame::TextEditFrame(QWidget *parent, ResultGrid *result_grid_widget, unsigned int index) :
    QFrame(parent)
{
  setMouseTracking(true);
  left_mouse_button_was_pressed= 0;
  ancestor_result_grid_widget= result_grid_widget;
  text_edit_frames_index= index;
  is_style_sheet_set_flag= false;
  hide();
}


TextEditFrame::~TextEditFrame()
{
}


void TextEditFrame::mousePressEvent(QMouseEvent *event)
{
  if (event->button() == Qt::LeftButton)
  {
    left_mouse_button_was_pressed= 1;
  }
}


void TextEditFrame::mouseMoveEvent(QMouseEvent *event)
{
  if (!(event->buttons() & Qt::LeftButton))
  {
    /* If cursor is over frame, set it to look like a point-both-ways arrow */
    /* Actuallly "> border_size won't happen, if mouse is on frame it's draggable. */
    /* Actually LEFT is impossible; we set left margin width= 0 earlier */
    /* if (event->x() <= border_size) {
       widget_side= LEFT;
       setCursor(Qt::SizeHorCursor); }
    else */
    if (event->x() >= width() - border_size)
    {       /* perhaps this should be 1 rather than border_size? */
      widget_side= RIGHT;
      setCursor(Qt::SizeHorCursor);
    }
    else if (event->y() >= height() - border_size)
    {       /* perhaps this should be 1 rather than border_size? */
      widget_side= BOTTOM;
      setCursor(Qt::SizeVerCursor);
    }
  }
  if (left_mouse_button_was_pressed == 0) return;      /* if mouse left button was never pressed, or was released, do nothing */
    /* if (widget_side == LEFT) {
        if ((width() - event->x()) > minimum_width) {
          setFixedSize(width() - event->x(),height()); } } */
  if (widget_side == RIGHT)
  {
    if (event->x() > minimum_width)
    {
      if (ancestor_result_grid_widget->ocelot_result_grid_vertical_copy > 0)
      {
        setFixedSize(event->x(), height());
      }
      else
      {
        /*  Now you must persuade ResultGrid to update all the rows. Beware of multiline rows and header row (row#0). */
        ancestor_result_grid_widget->grid_column_widths[ancestor_grid_column_number]= event->x();
        int xheight;
        for (long unsigned int xrow= 0;
             (xrow < ancestor_result_grid_widget->result_row_count + 1) && (xrow < RESULT_GRID_WIDGET_MAX_HEIGHT);
             ++xrow)
        {
          TextEditFrame *f= ancestor_result_grid_widget->text_edit_frames[xrow * ancestor_result_grid_widget->result_column_count + ancestor_grid_column_number];
          if (xrow > 0) xheight= ancestor_result_grid_widget->grid_column_heights[ancestor_grid_column_number];
          if (xrow == 0) xheight= f->height();
          f->setFixedSize(event->x(), xheight);
        }
      }
    }
  }
  if (widget_side == BOTTOM)
  {
    if (event->y() > minimum_height)
    {
      if (ancestor_result_grid_widget->ocelot_result_grid_vertical_copy > 0)
      {
        setFixedSize(width(), event->y());
      }
      else

      /*
        The following extra 'if' exists because Qt was displaying warnings like
        "QWidget::setMinimumSize: (/TextEditFrame) Negative sizes (-4,45) are not possible"
        when someone grabs the bottom and drags to the left.
        todo: find out why it doesn't seem to happen for ocelot_result_grid_vertical, see above.
      */
      if (event->x() >= minimum_width)
      {

        {
          /* todo: try to remember why you're looking at event->x() when change is to event->y()) */
          /*  Now you must persuade ResultGrid to update all the rows. Beware of multiline rows and header row (row#0). */
          ancestor_result_grid_widget->grid_column_heights[ancestor_grid_column_number]= event->y();
          int xheight;
          for (long unsigned int xrow= 0;
               (xrow < ancestor_result_grid_widget->result_row_count + 1) && (xrow < RESULT_GRID_WIDGET_MAX_HEIGHT);
               ++xrow)
          {
            TextEditFrame *f= ancestor_result_grid_widget->text_edit_frames[xrow * ancestor_result_grid_widget->result_column_count + ancestor_grid_column_number];
            if (xrow > 0) xheight= ancestor_result_grid_widget->grid_column_heights[ancestor_grid_column_number];
            if (xrow == 0) xheight= f->height();
            f->setFixedSize(event->x(), xheight);
          }
        }
      }
    }
  }
}


void TextEditFrame::mouseReleaseEvent(QMouseEvent *event)
{
  if (!(event->buttons() != 0)) left_mouse_button_was_pressed= 0;
}


/*
  This is an event that happens if a result-set grid cell comes into view due to scrolling.
  It can happen multiple times, and it can happen before we're ready to do anything (mysteriously).
  But it's great for handling result sets that have many cells, because some actions are slow.
  For example, setStyleSheet takes 2+ seconds when there are hundreds of child widgets.
  Todo: only apply setStyleSheet if new cell or if style change
*/
void TextEditFrame::paintEvent(QPaintEvent *event)
{
  if (event == 0) return; /* this is just to avoid an "unused parameter" warning */
  if (ancestor_result_grid_widget->is_paintable == 1)
  {
    /*
      Sometimes spurious text_edit_frames show up as if ready to paint.
      Todo: find out if there's a known Qt bug that might explain this.
      It's possible that it no longer happens now that I'm saying "hide()" more often.
    */
    if (text_edit_frames_index >= ancestor_result_grid_widget->max_text_edit_frames_count)
    {
      printf("Trying to paint a texteditframe that isn't in the layout\n");
      printf("  text_edit_frames_index=%d\n", text_edit_frames_index);
      printf("  max_text_edit_frames_count=%d\n", ancestor_result_grid_widget->max_text_edit_frames_count);
    }
    else
    {
      TextEditWidget *text_edit= findChild<TextEditWidget *>();
      if (text_edit != 0)
      {
        if (is_style_sheet_set_flag == false)
        {
          setStyleSheet(ancestor_result_grid_widget->frame_color_setting); /* for drag line color */
          if (ancestor_grid_row_number == -1) text_edit->setStyleSheet(ancestor_result_grid_widget->copy_of_parent->ocelot_grid_header_style_string);
          else text_edit->setStyleSheet(ancestor_result_grid_widget->copy_of_parent->ocelot_grid_style_string);
          is_style_sheet_set_flag= true;
        }
        if (is_retrieved_flag == false)
        {
          if (is_image_flag == false)
          {
            if (pointer_to_content == 0) text_edit->setText(QString::fromUtf8(NULL_STRING, sizeof(NULL_STRING) - 1));
            else text_edit->setText(QString::fromUtf8(pointer_to_content, length));
          }
          is_retrieved_flag= true;
        }
      }
    }
  }
}

/*
  TextEditWidget
  This is one of the components of result_grid
*/
TextEditWidget::TextEditWidget(QWidget *parent) :
    QTextEdit(parent)
{
}


TextEditWidget::~TextEditWidget()
{
}

/*
 Finally we're ready to paint a cell inside a frame inside a grid row inside result widget.
 The final decision is: paint as text (default) or paint as image (if blob and if flag).
 If it's a non-blob or if ocelot_blob_flag (?? check this name) is off: default paint.
 If it's a blob and ocelot_blob_flag is on:
   make a pixmap from the contents
   draw the pixmap
   todo: we're allowing resizing (miraculously) but there's no option for scrolling
   todo: consider: is there some better way to decide whether or not to display as image?
*/

void TextEditWidget::paintEvent(QPaintEvent *event)
{
  if (text_edit_frame_of_cell->is_image_flag == false)
  {
    QTextEdit::paintEvent(event);
    return;
  }
  //QPixmap p= QPixmap(QSize(event->rect().width(), event->rect().height()));
  QPixmap p= QPixmap();
  if (p.loadFromData((const uchar*) text_edit_frame_of_cell->pointer_to_content,
                     text_edit_frame_of_cell->length,
                     0,
                     Qt::AutoColor) == false)
  {
    if (text_edit_frame_of_cell->pointer_to_content != 0)
    {
      setText(QString::fromUtf8(text_edit_frame_of_cell->pointer_to_content,
                                text_edit_frame_of_cell->length));
    }
    QTextEdit::paintEvent(event);
    return;
  }
  QPainter painter(this->viewport());
  /* Todo: this will scale to the size of the cell. Better would be: fragment + scrollbar. */
  painter.drawPixmap(event->rect(), p);
  return;
}

/*
  CONNECT
*/

/*
   MySQL options
   MySQL's client library has routines for a consistent way to see what
   options the user has put in a configuration file such as my.cnf, or added
   on the command line with phrases such as --port=x.
   Todo: meld that with whatever a user might say in a CONNECT command line
         or maybe even a dialog box.
   Assume mysql_init() has already happened. (Currently it's in this function, but it shouldn't be.)
   Qt gets to see argc+argv first, and Qt will process options that it recognizes
   such as -style, -session, -graphicssystem. See
   http://qt-project.org/doc/qt-4.8/qapplication.html#details.
   But when Qt is done, it should remove what it parsed.
   Todo: actually you should operate on a copy of argc + argv, rather than change the originals. QT docs say so.
         but that would contradict another todo, which is to blank the password if it's in an argv
   Most options are ignored but the ones which might be essential for connecting are not ignored.
   Example: if ~/.my.cnf has "port=3306" in [clients] group, and start happens with --host=127.0.0.1,
            then port=3306 and current_host=127.0.0.1 and that will get passed to the connect routine.
   Read: http://dev.mysql.com/doc/refman/5.6/en/connecting.html
         See also http://dev.mysql.com/doc/refman/5.6/en/mysql-command-options.html

   At one time there was an include of getopt.h and calls to my_long_options etc.
   That introduced some unwanted dependencies and so now we do all the option retrievals directly.
 */

/*
  Connect -- get options

  Environment variables, then option files, then command-line arguments

  Except that you have to read the command-line arguments to know whether
  you should read the option files, see http://dev.mysql.com/doc/refman/5.7/en/option-file-options.html

  For environment variables:
  Follow http://dev.mysql.com/doc/refman/5.7/en/environment-variables.html
  * Watch: LD_RUN_PATH MYSQL_GROUP_SUFFIX MYSQL_HOST MYSQL_PS1 MYSQL_PWD MYSQL_TCP_PORT TZ USER

  For option files:
  Follow MySQL manual http://dev.mysql.com/doc/refman/5.7/en/option-files.html
  Don't follow MariaDB documentation https://mariadb.com/kb/en/mariadb/mariadb-documentation/getting-started/getting-installing-and-upgrading-mariadb/configuring-mariadb-with-mycnf/
  Todo: Find out whether MariaDB documentation is merely wrong, or represents a real incompatibility with MySQL.
  * Read these files:
  /etc/my.cnf
  /etc/mysql/my.cnf
  SYSCONFDIR/my.cnf                          ?? i.e. [installation-directory]/etc/my.cnf but this should be changeable
  $MYSQL_HOME/my.cnf
  file specified with --defaults-extra-file
  $HOME/.my.cnf             MySQL manual says ~/.my.cnf which isn't necessarily the same thing, but I don't believe that
  $HOME/.mylogin.cnf        MySQL manual says ~/.mylogin.cnf which isn't necessarily the same thing, but I don't believe that
  ... There is no use of DATADIR, which is deprecated, and (I think) would only relate to server anyway.
  Todo: On Unix, "ignore configuration files that are world-writable".
  * How to read ...
  for (;;)
    read line
    tokenize for the line
    If blank, or starts with '#', or starts with ';', ignore
    ignore everything following '#' (not necessarily at start of line)
    start only when you see [client] or [mysql] or [ocelot]
    stop when you see [something-else]
    expand escape sequences
    if you see opt_name = value:
      compare to important-opt-name but allow "unambiguous prefix" see http://dev.mysql.com/doc/refman/5.7/en/program-options.html
      if opt_name is important
        get value, strip '' or "", strip lead/trail
        assign: variable-name-corresponding-to-opt-name = value
    if you see !include /home/mydir/myopt.cnf or the like:
        open the file; subsequent read lines will be from there, but you have to return
        [ if you're already in an include, that's probably an error ]
    if you see !includedir /home/mydir or the like:
        each file in the directory might be an options file, ending in ".cnf"
  * Last specification takes precedence, so we're starting with
  (say) a variable named OPT which is blank, setting it to
  something when we see OPT = xxx, and setting it again when
  we see OPT = xxx again. Only PASSWORD is tricky, because it
  can be followed by blank to indicate password is necessary.

  For command-line:
  Follow http://dev.mysql.com/doc/refman/5.7/en/command-line-options.html
  * This is somewhat different from the tokenizing that's done for the
    command line, but it's still a matter of tokenizing
  * The trick is: you have to tokenize twice, before and after command-line stuff
  * Get rid of any arguments that you actually handle, Qt might want the rest
  * todo: recognize --print-defaults and --login-pathconf

  In the end, you have a bunch of variables with values.
  Todo: For each variable with a value, append to "CONNECT" client-statement,
  which eventually will be what you actually use (though password won't be repeated?).
  See connect_make_statement().

  Todo: mysql client handles bad format in my.cnf file thus:
        "error: Found option without preceding group in config file: /home/pgulutzan/.my.cnf at line: 1
         Fatal error in defaults handling. Program aborted"
        Maybe we should do the same?

  The above comments apply to Unix, Windows is a bit different.

  Todo: whenever you have a token1/token2/token3:
        find token1 in case it's one of the options we track
        set the value for that option
  Todo: if argv[i] is a password, you should wipe it out. Compare what mysql client does:
        "while (*argument) *argument++= 'x';"

  Todo: The environment variable LD_LIBRARY_PATH is also important, because it determines
        where libmysqlclient.so and the Qt libraries will be loaded from. Document that,
        and show it somewhere.
        The only complication I can think of is that, if I
        decide someday to support another DBMS, the MySQL
        stuff would still come in or have to be disabled.
*/

#include <dirent.h>

void connect_set_variable(QString token0, QString token2);
void connect_read_command_line(int argc, char *argv[]);
void connect_read_my_cnf(const char *file_name);
QString connect_stripper(QString value_to_strip);

#include <pwd.h>
#include <unistd.h>

void MainWindow::connect_mysql_options_2(int argc, char *argv[])
{
  char *mysql_pwd;
  char *home;
  char tmp_my_cnf[1024];                         /* file name = $HOME/.my.cnf or $HOME/.mylogin.cnf or defaults_extra_file */

  /*
    Todo: check: are there any default values to be set before looking at environment variables?
          I don't see documentation to that effect, so I'm just setting them to "" or 0.
          Except that port initially = MYSQL_PORT (probably 3306), and ocelot_delimiter_str initially = ";"
          and http://dev.mysql.com/doc/refman/5.6/en/connecting.html mentions a few others:
          host = 'localhost' (which means protocol=SOCKET if mysql client, but we ignore that)
          user = Unix login name on Linux, although on Windows it would be 'ODBC'
          and there seem to be some getenv() calls in mysqlcc that I didn't take into account.
  */
  ocelot_no_defaults= 0;
  ocelot_defaults_file= "";
  ocelot_defaults_extra_file= "";

  ocelot_host= "localhost";
  ocelot_database= "";
  ocelot_user= "";
  ocelot_password_was_specified= 0;
  /* ocelot_port= MYSQL_PORT; */ /* already initialized */
  ocelot_unix_socket= "";
  ocelot_default_auth= "";
  /* ocelot_enable_cleartext_plugin= 0; */ /* already initialized */
  ocelot_init_command= "";
  ocelot_opt_bind= "";
  /* ocelot_opt_can_handle_expired_passwords= 0; */ /* already initialized */
  /* ocelot_opt_compress= 0; */ /* already initialized */
  ocelot_opt_connect_attr_delete= "";
  /* ocelot_opt_connect_attr_reset= 0; */ /* already initialized */
  /* ocelot_opt_connect_timeout= 0; */ /* ocelot_opt_connect_timeout is initialized to 0 */
  /* ocelot_opt_local_infile= 0; */ /* ocelot_opt_local_infile is initialized to 0 */
  /* ocelot_opt_named_pipe= 0; */ /* already initialized */
  /* ocelot_opt_read_timeout= 0; */ /* already initialized */
  /* ocelot_opt_reconnect= 0; */ /* ocelot_opt_reconnect is initialized to 0 */
  ocelot_opt_ssl= "";
  ocelot_opt_ssl_ca= "";
  ocelot_opt_ssl_capath= "";
  ocelot_opt_ssl_cert= "";
  ocelot_opt_ssl_cipher= "";
  ocelot_opt_ssl_crl= "";
  ocelot_opt_ssl_crlpath= "";
  ocelot_opt_ssl_key= "";
  /* ocelot_opt_ssl_verify_server_cert= 0; */ /* already initialized */
  /* ocelot_opt_use_result= 0; */ /* already initialized */
  /* ocelot_opt_write_timeout= 0; */ /* already initialized */
  ocelot_plugin_dir= "";
  ocelot_read_default_file= "";
  ocelot_read_default_group= "";
  /* ocelot_report_data_truncation= 0; */ /* already initialized */
  /* ocelot_secure_auth= 1; secure_auth is 1 by default anyway */ /* =1 if mysql 5.6.7 */
  ocelot_server_public_key= "";
  ocelot_set_charset_dir= "";
  ocelot_set_charset_name= ""; /* ocelot_default_character_set= "";  */
  ocelot_shared_memory_base_name= "";

  ocelot_protocol= ""; ocelot_protocol_as_int= get_ocelot_protocol_as_int(ocelot_protocol);

  ocelot_delimiter_str= ";";
  /* ocelot_history_includes_warnings= 0; is default anyway */
  /* ocelot_sigint_ignore= 0; is default anyway */
  /* ocelot_safe_updates= 0; */ /* ocelot_safe_updates is initialized to 0 */
  /* ocelot_select_limit= 0; */ /* ocelot_select_limit is initialized to 0 */
  /* ocelot_max_join_size= 0; */ /* ocelot_max_join_size is initialized to 0 */
  /* ocelot_silent= 0; */ /* ocelot_silent is initialized to 0 */
  /* ocelot_no_beep= 0; */ /* ocelot_no_beep is initialized to 0 */
  /* ocelot_wait= 0; *//* ocelot_wait is initialized to 0 */

  ocelot_bind_address= "";
  ocelot_debug= "";
  ocelot_execute= "";
  ocelot_login_path= "";
  ocelot_pager= "";
  ocelot_prompt= "mysql>";                  /* Todo: change to "\N [\d]>"? */

  {
    struct passwd *pw;
    uid_t u;
    u= geteuid();
    pw= getpwuid(u);
    if (pw != NULL) ocelot_user= pw->pw_name;
  }

  connect_read_command_line(argc, argv);               /* We're doing this twice, the first time won't count. */

  /* Environment variables */
  //ld_run_path= getenv("LD_RUN_PATH");                /* We're not doing anything with this yet. Maybe we should. */
  //group_suffix= getenv("GROUP_SUFFIX");              /* "" */
  //mysql_host= getenv("MYSQL_HOST");                  /* "" */
  //mysql_ps1= getenv("MYSQL_PS1");                    /* "" */
  if (getenv("MYSQL_PWD") != 0)
  {
    mysql_pwd= getenv("MYSQL_PWD");
    ocelot_password= mysql_pwd;
    ocelot_password_was_specified= 1;
  }
  if (getenv("MYSQL_TCP_PORT") != 0) ocelot_port= atoi(getenv("MYSQL_TCP_PORT"));         /* "" */
  //tz_user= getenv("TZ_USER");
  home= getenv("HOME");

  /*
    Options files i.e. Configuration files i.e. my_cnf files
    Don't read option files if ocelot_no_defaults==1 (which is true if --no-defaults was specified on command line).
    Todo: check: does MariaDB read mylogin.cnf even if ocelot_no_defaults==1?
  */
  if (QString::compare(ocelot_defaults_file, " ") > 0)
  {
    strcpy(tmp_my_cnf, ocelot_defaults_file.toUtf8());
    connect_read_my_cnf(tmp_my_cnf);
  }
  else
  {
    if (ocelot_no_defaults == 0)
    {
      connect_read_my_cnf("/etc/my.cnf");
      connect_read_my_cnf("/etc/mysql/my.cnf");
      /* todo: think: is argv[0] what you want for SYSCONFDIR? not exact, but it's where the program is now. no, it might be a copy. */
      // connect_read_my_cnf("SYSCONFDIR/etc/my.cnf") /* ?? i.e. [installation-directory]/etc/my.cnf but this should be changeable */
      /* skip $MYSQL_HOME/my.cnf, only server stuff should be in it */
      // connect_read_my_cnf("file specified with --defaults-extra-file");
      if (QString::compare(ocelot_defaults_extra_file, " ") > 0)
      {
        strcpy(tmp_my_cnf, ocelot_defaults_extra_file.toUtf8());
        connect_read_my_cnf(tmp_my_cnf);
      }
      strcpy(tmp_my_cnf, home);                              /* $HOME/.my.cnf */
      strcat(tmp_my_cnf, "/.my.cnf");
      connect_read_my_cnf(tmp_my_cnf);
    }
    strcpy(tmp_my_cnf, home);                          /* $HOME/.mylogin.cnf */
    strcat(tmp_my_cnf, "/.mylogin.cnf");
    connect_read_my_cnf(tmp_my_cnf);
  }
  connect_read_command_line(argc, argv);
  //connect_make_statement();
}


/*
  Command line arguments i.e. argc + argv
  Some tokenizing has already been done.
  For example progname --a=b --c=d gives us argv[1]="--a=b" and argv[2]="--c=d".
  For example progname -a b -c d gives us argv[1]="-a" argv[2]="b" argv[3]="-c" argv[4]="d".
  If something is enclosed in single or double quotes, then it has already been stripped of quotes and lead/trail spaces.
  todo: parse so -p x and --port=x etc. are known
  todo: check: is it okay still to abbreviate e.g. us rather than user?
  todo: strip the arguments so Qt doesn't see them, or maybe don't
  todo: do command line arguments TWICE! first time is just to find out what my.cnf files should be read, etc.
        second time is to override whatever happened during getenv and option-file processing
  todo: you seem to be forgetting that Qt can also expect command-line options
*/
void MainWindow::connect_read_command_line(int argc, char *argv[])
{
  QString token0, token1, token2;
  QString s_argv;
  int i;

  for (i= 1; i < argc; ++i)
  {
    s_argv= argv[i];
    /* If there's no '-' then (I guess) this is the database name */
    if (s_argv.mid(0, 1) != "-")
    {
      token0= "database"; token1= "="; token2= s_argv;
    }
    /* If there is one '-' then token1=argv[i], token2='=', token3=argv[i+1 */
    else if ((strlen(argv[i]) > 1) && (s_argv.mid(1, 1) != "-"))
    {
      token0= argv[i]; token1= ""; token2= "";
      if (token0 == "-b") token0= "no_beep";
      if (token0 == "-C") token0= "compress";
      if (token0 == "-c") token0= "comments";
      if (token0 == "-D") token0= "database";
      if (token0 == "-E") token0= "vertical";
      if (token0 == "-h") token0= "host";
      if (token0 == "-P") token0= "port";
      if (token0 == "-p") token0= "password";
      if (token0 == "-S") token0= "socket";
      if (token0 == "-s") token0= "silent";
      if (token0 == "-U") token0= "safe_updates";
      if (token0 == "-u") token0= "user";
      if (token0 == "-w") token0= "wait";
      if (i < argc - 1)
      {
        token1= "="; token2= argv[i + 1];
        ++i;
      }
    }
    /* If there are two '-'s then token1=argv[i] left, token2='=', token3=argv[i] right */
    else
    {
      QString equaller;
      int argv_length;
      argv_length= strlen(argv[i]);
      equaller= "=";
      int equaller_index;
      equaller_index= s_argv.indexOf(equaller);
      if (equaller_index == -1)
      {
        token0= s_argv.mid(2, argv_length - 2);
        token1= "";
        token2= "";
      }
      else
      {
        token0= s_argv.mid(2, equaller_index - 2);
        token1= "=";
        token2= s_argv.mid(equaller_index + 1, (argv_length - equaller_index) - 1);
      }
    }
    connect_set_variable(token0, token2);
  }
}


/* Todo: unfortunately MAX_TOKENS == 10000, when we only need 10, so there's temporary space waste
         maybe we should use main_token_offsets and main_token_lengths? */
/* todo: check if we've already looked at the file (this is possible if !include or !includedir happens)
         if so, skip */
void MainWindow::connect_read_my_cnf(const char *file_name)
{
  FILE *file;
  char line[2048];
  int token_offsets[MAX_TOKENS];
  int token_lengths[MAX_TOKENS];
  int i;
  QString token0, token1, token2, token_for_value;
  QString group;                                         /* what was in the last [...] e.g. mysqld, client, mysql, ocelot */
  int token0_length, token1_length, token2_length;

  group= "";                                             /* group identifier doesn't carry over from last .cnf file that we read */
  file= fopen(file_name, "r");                           /* Open specified file, read only */
  if (file == NULL)                                      /* (if file doesn't exist, ok, no error */
  {
    return;
  }
  while(fgets(line, sizeof line, file) != NULL)
  {
    QString s= line;
    /* tokenize, ignore # comments or / * comments * /, treat '-' as part of token not operator */
    tokenize(s.data(),
             s.size(),
             &token_lengths, &token_offsets, MAX_TOKENS,(QChar*)"33333", 2, "", 2);
    /* Ignore blank lines and lines that start with ';' */
    if (token_lengths[0] == 0) continue;
    if (QString::compare(s.mid(token_offsets[0], token_lengths[0]), ";", Qt::CaseInsensitive) == 0) continue;
    /* Possible meaningful lines are: [ group ], ! include x, ! includedir x, variable = value */
    token0_length= token_lengths[0];
    token0= s.mid(token_offsets[0], token0_length);
    token1_length= token_lengths[1];
    if (token1_length != 0) token1= s.mid(token_offsets[1], token1_length);
    else token1= "";
    if (token1_length == 0) token_lengths[2]= 0;
    token2_length= token_lengths[2];
    if (token2_length != 0)
    {
      int last_token;
      for (last_token= 3; token_lengths[last_token] != 0; ++last_token) ;
      token2_length= token_offsets[last_token - 1] + token_lengths[last_token - 1] - token_offsets[2];
      token2= s.mid(token_offsets[2], token2_length);
    }
    else token2= "";

    /* See if it's !include + file-name */
    /* todo: check for a sym link, or a directory. I don't know what we should do with such junk. */
    if ((QString::compare(token0, "!") == 0) && (QString::compare(token1, "include", Qt::CaseInsensitive) == 0))
    {
      char new_file_name[2048];
      strcpy(new_file_name,token2.toUtf8());
      *(new_file_name + token2_length)= 0;
      connect_read_my_cnf(new_file_name);
    }
    /* See if it's !includedir */
    if ((QString::compare(token0, "!") == 0) && (QString::compare(token1, "includedir", Qt::CaseInsensitive) == 0))
    {
      DIR *d;
      struct dirent *dir;
      char new_directory_name[2048];
      strcpy(new_directory_name,token2.toUtf8());
      *(new_directory_name + token2_length)= 0;
      d= opendir(new_directory_name);
      if (d)
      {
        while ((dir = readdir(d)) != NULL)
        {
          if ((strlen(dir->d_name)>4) && (strcmp(dir->d_name + strlen(dir->d_name) - 4, ".cnf") == 0))
          {
            char new_file_name[2048];
            strcpy(new_file_name, new_directory_name);
            strcat(new_file_name, "/");
            strcat(new_file_name, dir->d_name);
            connect_read_my_cnf(new_file_name);
          }
        }
        closedir(d);
      }
    }

    /* See if it's [ group ] */
    if ((QString::compare(token0, "[") == 0) && (QString::compare(token2,"]") == 0))
    {
      group= token1;
      continue;
    }
    /* Skip if it's not one of the groups that we care about i.e. client or mysql or ocelot */
    if ((QString::compare(group, "client", Qt::CaseInsensitive) != 0)
    &&  (QString::compare(group, "mysql", Qt::CaseInsensitive) != 0)
    &&  (QString::compare(group, "ocelot", Qt::CaseInsensitive) != 0)) continue;

    /* Remove ''s or ""s around the value, then strip lead or trail spaces. */
    token2= connect_stripper(token2);
    token2_length= token2.count();

    /* Convert escape sequences in the value \b backspace, \t tab, \n newline, \r carriage return, \\ \, \s space */
    token_for_value= "";
    for (i= 0; i < token2_length; ++i)
    {
      QString c, c2;
      c= token2.mid(i, 1);
      if ((c == "\\") && (i < token2_length))
      {
        c2= token2.mid(i + 1, 1);
        if (c2 == "b") { c2= 0x08; token_for_value = token_for_value + c2; ++i; continue; }
        if (c2 == "t"){ c2= 0x09; token_for_value = token_for_value + c2; ++i; continue; }
        if (c2 == "n"){ c2= 0x0a; token_for_value = token_for_value + c2; ++i; continue; }
        if (c2 == "r") { c2= 0x0d; token_for_value = token_for_value + c2; ++i; continue; }
        if (c2 == "\\") { c2= "\\"; token_for_value = token_for_value + c2; ++i; continue; }
        if (c2 == "s") { c2= 0x20; token_for_value = token_for_value + c2; ++i; continue; }
      }
      token_for_value= token_for_value + c;
    }
    token2= token_for_value;
    connect_set_variable(token0, token2);
  }
  fclose(file);
}


/*
  Remove ''s or ""s or ``s around a QString, then remove lead or trail spaces.
  Called for connect, and also for things like USE `test`.
  I didn't say remove lead or trail whitespace, so QString "trimmed()" is no good.
  todo: I'm not sure that `` (tildes) should be removed for my.cnf values, check that
        (we are depending on stripping of ``s when we call connect_stripper() with debuggee information status values)
  todo: I am fairly sure that I need to call this from other places too.
  todo: Does not look for 'xxx''yyy'. should it?
*/
QString MainWindow::connect_stripper(QString value_to_strip)
{
  QString s;
  int s_length;
  char c_singlequote[2];
  char c_doublequote[2];
  char c_grave_accent[2]; /* Grave Accent is the Unicode term for `. MySQL calls it backtick. */

  s= value_to_strip;
  s_length= s.count();
  if (s_length > 1)
  {
    c_singlequote[0]= 0x27; c_singlequote[1]= 0;
    c_doublequote[0]= 0x22; c_doublequote[1]= 0;
    c_grave_accent[0]= 0x60; c_grave_accent[1]= 0;
    if (((s.mid(0, 1) == QString(c_singlequote)) && (s.mid(s_length - 1, 1) == QString(c_singlequote)))
    ||  ((s.mid(0, 1) == QString(c_doublequote)) && (s.mid(s_length - 1, 1) == QString(c_doublequote)))
    ||  ((s.mid(0, 1) == QString(c_grave_accent)) && (s.mid(s_length - 1, 1) == QString(c_grave_accent))))
    {
      s= s.mid(1, s_length - 2);
      while ((s.count() > 0) && (s.mid(0, 1) == " "))
      {
        s_length= s.count();
        s= s.mid(1, s_length - 1);
      }
      while ((s.count() > 0) && (s.mid(s.count() - 1, 1) == " "))
      {
        s_length= s.count();
        s= s.mid(0, s_length - 1);
      }
    }
  }
  return s;
}


/*
  Given token1=option-name [token2=equal-sign token3=value],
  see if option-name corresponds to one of your program-variables,
  and if so set program-variable = true or program-variable = value.
  For example, if token1="user", token2="=", token3="peter",
  then set ocelot_user = "peter".
  But that would be too simple, eh? So here are some complications:
  * unambiguous prefixes of option names are allowed until MySQL 5.7
    (this is true for options but not true for values)
    http://dev.mysql.com/doc/refman/5.6/en/program-options.html
  * '-' and '_' are interchangeable
    http://dev.mysql.com/doc/refman/5.6/en/command-line-options.html
  * todo: if and only if the target is numeric, then before converting
    the string to a number check whether it ends with K or M or G, and
    if it does then multiply by a kilo or a mega or a giga
  option our variable name mysql variable name
  ------ ----------------- -------------------
  ho[st] ocelot_host       current_host
  us[er] ocelot_user       current_user
  database ocelot_database current_db
  so[cket] ocelot_unix_socket                opt_mysql_unix_port
  po[rt]   ocelot_port     mysql opt_mysql_port
  comm[ents] ocelot_comments preserve_comments
  ocelot_init_command opt_init_command
  ocelot_default_auth opt_default_auth
  no-defaults ocelot_no_defaults
  defaults-extra-file ocelot_defaults_extra_file
  defaults-file ocelot_defaults_file
  delimiter ocelot_delimiter_str
  show-warnings ocelot_history_includes_warnings
  connect_timeout ocelot_opt_connect_timeout
  compress ocelot_opt_compress
  secure_auth ocelot_secure_auth
  local_infile ocelot_opt_local_infile
  safe_updates or i_am_a_dummy ocelot_safe_updates
  plugin_dir ocelot_plugin_dir
  select_limit ocelot_select_limit
  max_join_size ocelot_max_join_size
  silent ocelot_silent
  no_beep ocelot_no_beep
  wait ocelot_wait
  default-character-set ocelot_default_character_set
*/
void MainWindow::connect_set_variable(QString token0, QString token2)
{
  unsigned int token0_length;
  char token0_as_utf8[64 + 1];

  token0_length= token0.count();
  if (token0_length >= 64) return; /* no option name is ever longer than 64 bytes */
  strcpy(token0_as_utf8,token0.toUtf8());
  *(token0_as_utf8 + token0_length)= 0;
  for (int i= 0; token0_as_utf8[i] != '\0'; ++i)
  {
    if (token0_as_utf8[i] == '-') token0_as_utf8[i]= '_';
  }

  if (strcmp(token0_as_utf8, "auto_rehash") == 0) { ocelot_auto_rehash= 1; return; }
  if (strcmp(token0_as_utf8, "auto_vertical_output") == 0) { ocelot_auto_vertical_output= 1; return; }
  if (strcmp(token0_as_utf8, "batch") == 0) { ocelot_batch= 1; return; }
  if (strcmp(token0_as_utf8, "binary_mode") == 0) { ocelot_binary_mode= 1; return; }
  if (strcmp(token0_as_utf8, "bind") == 0) /* not available in mysql client */
  {
    ocelot_opt_bind= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "bind_address") == 0) { ocelot_bind_address= 1; return; }
  if (strcmp(token0_as_utf8, "can_handle_expired_passwords") == 0) /* not available in mysql client */
  {
    ocelot_opt_can_handle_expired_passwords= 1;
    return;
  }

  if (strcmp(token0_as_utf8, "character_sets_dir") == 0)
  {
    ocelot_set_charset_dir= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "column_names") == 0) { ocelot_result_grid_column_names= 1; return; }
  if (strcmp(token0_as_utf8, "column_type_info") == 0) { ocelot_column_type_info= 1; return; }
  if ((token0_length >= sizeof("comm")) && (strncmp(token0_as_utf8, "comments", token0_length) == 0))
  {
    ocelot_comments= 1;
    return;
  }
  if ((token0_length >= sizeof("comp")) && (strncmp(token0_as_utf8, "compress", token0_length) == 0))
  {
    ocelot_opt_compress= 1;
    return;
  }
  if (strcmp(token0_as_utf8, "connect_attr_delete") == 0) /* not available in mysql client */
  {
    ocelot_opt_connect_attr_delete= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "connect_attr_reset") == 0) /* not available in mysql client */
  {
    ocelot_opt_connect_attr_reset= 1;
    return;
  }
  if ((token0_length >= sizeof("con")) && (strncmp(token0_as_utf8, "connect_timeout", token0_length) == 0))
  {
    ocelot_opt_connect_timeout= token2.toLong();
    return;
  }
  if (strcmp(token0_as_utf8, "database") == 0)
  {
    ocelot_database= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "debug") == 0) { ocelot_debug= token2; return; }
  if (strcmp(token0_as_utf8, "debug_info") == 0) { ocelot_debug_info= 1; return; }
  if ((token0_length >= sizeof("default_a")) && (strncmp(token0_as_utf8, "default_auth", token0_length) == 0))
  {
    ocelot_default_auth= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "default_character_set") == 0)
  {
    ocelot_set_charset_name= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "defaults_extra_file") == 0)
  {
    ocelot_defaults_extra_file= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "defaults_file") == 0)
  {
    ocelot_defaults_file= token2;
    return;
  }
  if ((token0_length >= sizeof("del")) && (strncmp(token0_as_utf8, "delimiter", token0_length) == 0))
  {
    ocelot_delimiter_str= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "disable_named_commands") == 0) { ocelot_named_commands= 0; return; }
  if (strcmp(token0_as_utf8, "enable_cleartext_plugin") == 0)
  {
    ocelot_enable_cleartext_plugin= 1;
    return;
  }
  if (strcmp(token0_as_utf8, "execute") == 0) { ocelot_execute= token2; return; }
  if (strcmp(token0_as_utf8, "force") == 0) { ocelot_force= 1; return; }
  if (strcmp(token0_as_utf8, "help") == 0) { ocelot_help= 1; return; }
  if (strcmp(token0_as_utf8, "histignore") == 0) { ocelot_histignore= 1; return; }
  if ((token0_length >= sizeof("ho")) && (strncmp(token0_as_utf8, "host", token0_length) == 0))
  {
    ocelot_host= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "html") == 0) { ocelot_html= 1; return; }
  if ((token0_length >= sizeof("i_")) && (strncmp(token0_as_utf8, "i_am_a_dummy", token0_length) == 0))
  {
    ocelot_safe_updates= 1;
    return;
  }
  if (strcmp(token0_as_utf8, "ignore_spaces") == 0) { ocelot_ignore_spaces= 1; return; }
  if (strcmp(token0_as_utf8, "init_command") == 0)
  {
    ocelot_init_command= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "line_numbers") == 0) { ocelot_line_numbers= 1; return; }
  if ((token0_length >= sizeof("lo")) && (strncmp(token0_as_utf8, "local_infile", token0_length) == 0))
  {
    ocelot_opt_local_infile= 1;
    return;
  }
  if (strcmp(token0_as_utf8, "login_path") == 0) { ocelot_login_path= token2; return; }
  if (strcmp(token0_as_utf8, "max_allowed_packet") == 0) { ocelot_max_allowed_packet= token2.toLong(); return; }
  if ((token0_length >= sizeof("max_j")) && (strncmp(token0_as_utf8, "max_join_size", token0_length) == 0))
  {
    ocelot_max_join_size= token2.toLong();
    return;
  }
  if (strcmp(token0_as_utf8, "named_commands") == 0) { ocelot_named_commands= 1; return; }
  if (strcmp(token0_as_utf8, "net_buffer_length") == 0) { ocelot_net_buffer_length= token2.toLong(); return; }
  if (strcmp(token0_as_utf8, "no_auto_rehash") == 0) { ocelot_auto_rehash= 0; return; }
  if ((token0_length >= sizeof("no_b")) && (strncmp(token0_as_utf8, "no_beep", token0_length) == 0))
  {
    ocelot_no_beep= 1;
    return;
  }
  if (strcmp(token0_as_utf8, "no_defaults") == 0) { ocelot_no_defaults= 1; return; }
  if (strcmp(token0_as_utf8, "no_named_commands") == 0) { ocelot_named_commands= 0; return; }
  if (strcmp(token0_as_utf8, "no_tee") == 0) { history_tee_stop(); return; }/* for tee */
  if (strcmp(token0_as_utf8, "one_database") == 0) { ocelot_one_database= 1; return; }
  if (strcmp(token0_as_utf8, "pager") == 0) { ocelot_pager= 1; return; }
  if ((token0_length >= sizeof("pas")) && (strncmp(token0_as_utf8, "password", token0_length) == 0))
  {
    ocelot_password= token2;
    ocelot_password_was_specified= 1;
    return;
  }
  if (strcmp(token0_as_utf8, "pipe") == 0) /* Not sure about this. Windows. Same as protocol? */
  {
    ocelot_opt_named_pipe= 1;
    return;
  }
  if ((token0_length >= sizeof("pl")) && (strncmp(token0_as_utf8, "plugin_dir", token0_length) == 0))
  {
    ocelot_plugin_dir= token2;
    return;
  }
  if ((token0_length >= sizeof("po")) && (strncmp(token0_as_utf8, "port", token0_length) == 0))
  {
    ocelot_port= token2.toInt();
    return;
  }
  if (strcmp(token0_as_utf8, "print_defaults") == 0) { ocelot_print_defaults= 1; return; }
  if (strcmp(token0_as_utf8, "prompt") == 0) { ocelot_prompt= token2; return; }
  if ((token0_length >= sizeof("prot")) && (strncmp(token0_as_utf8, "protocol", token0_length) == 0))
  {
    ocelot_protocol= token2; /* Todo: perhaps make sure it's tcp/socket/pipe/memory */
    ocelot_protocol_as_int= get_ocelot_protocol_as_int(ocelot_protocol);
    return;
  }
  if (strcmp(token0_as_utf8, "quick") == 0) { ocelot_quick= 1; return; }
  if (strcmp(token0_as_utf8, "raw") == 0) { ocelot_raw= 1; return; }
  if (strcmp(token0_as_utf8, "read_default_file") == 0) /* not available in mysql client */
  {
    ocelot_read_default_file= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "read_default_group") == 0) /* not available in mysql client */
  {
    ocelot_read_default_group= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "read_timeout") == 0) /* not available in mysql client */
  {
    ocelot_opt_read_timeout= token2.toInt();
    return;
  }
  if (strcmp(token0_as_utf8, "reconnect") == 0)
  {
    ocelot_opt_reconnect= 1;
    return;
  }
  if (strcmp(token0_as_utf8, "report_data_truncation") == 0) /* not available in mysql client */
  {
    ocelot_report_data_truncation= 1;
    return;
  }
  if ((token0_length >= sizeof("sa")) && (strncmp(token0_as_utf8, "safe_updates", token0_length) == 0))
  {
    ocelot_safe_updates= 1;
    return;
  }
  if ((token0_length >= sizeof("sec")) && (strncmp(token0_as_utf8, "secure_auth", token0_length) == 0))
  {
    ocelot_secure_auth= 1;
    return;
  }
  if (strcmp(token0_as_utf8, "server_public_key") == 0) /* not available in mysql client */
  {
    ocelot_server_public_key= token2;
    return;
  }
   if ((token0_length >= sizeof("sel")) && (strncmp(token0_as_utf8, "select_limit", token0_length) == 0))
  {
    ocelot_select_limit= token2.toLong();
    return;
  }

  if (strcmp(token0_as_utf8, "shared_memory_base_name") == 0)
  {
    ocelot_shared_memory_base_name= token2;
    return;
    }
  if ((token0_length >= sizeof("sh")) && (strncmp(token0_as_utf8, "show_warnings", token0_length) == 0))
  {
    ocelot_history_includes_warnings= 1;
    return;
    }
  if (strcmp(token0_as_utf8, "sigint_ignore") == 0) { ocelot_sigint_ignore= 1; return; }
  if ((token0_length >= sizeof("sil")) && (strncmp(token0_as_utf8, "silent", token0_length) == 0))
  {
    ocelot_silent= 1;
    return;
  }
  if (strcmp(token0_as_utf8, "skip_auto_rehash") == 0) { ocelot_auto_rehash= 0; return; }
  if (strcmp(token0_as_utf8, "skip_column_names") == 0) { ocelot_result_grid_column_names= 0; return; }
  if (strcmp(token0_as_utf8, "skip_comments") == 0) { ocelot_comments= 0; return; }
  if (strcmp(token0_as_utf8, "skip_named_commands") == 0) { ocelot_named_commands= 0; return; }
  if (strcmp(token0_as_utf8, "skip_secure_auth") == 0)
  {
    ocelot_secure_auth= 0;
    return;
  }
  if ((token0_length >= sizeof("so")) && (strncmp(token0_as_utf8, "socket", token0_length) == 0))
  {
    ocelot_unix_socket= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl") == 0)
  {
    ocelot_opt_ssl= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_ca") == 0)
  {
    ocelot_opt_ssl_ca= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_capath") == 0)
  {
    ocelot_opt_ssl_capath= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_cert") == 0)
  {
    ocelot_opt_ssl_cert= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_cipher") == 0)
  {
    ocelot_opt_ssl_cipher= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_crl") == 0)
  {
    ocelot_opt_ssl_crl= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_crlpath") == 0)
  {
    ocelot_opt_ssl_crlpath= token2;
    return;
  }
  if (strcmp(token0_as_utf8, "ssl_key") == 0)
  {
    ocelot_opt_ssl_key= token2;
    return;
  }
  if ((token0_length >= sizeof("ssl_verify")) && (strncmp(token0_as_utf8, "ssl_verify_server_cert", token0_length) == 0))
  {
    ocelot_opt_ssl_verify_server_cert= token2.toInt();
    return;
  }
  if (strcmp(token0_as_utf8, "syslog") == 0) { ocelot_syslog= 1; return; }
  if (strcmp(token0_as_utf8, "table") == 0) { ocelot_table= 1; return; }
  if (strcmp(token0_as_utf8, "tee") == 0) { history_tee_start(token2); /* todo: check whether history_tee_start returned NULL which is an error */ return; }/* for tee */
  if (strcmp(token0_as_utf8, "unbuffered") == 0) { ocelot_unbuffered= 1; return; }
  if (strcmp(token0_as_utf8, "use_result") == 0) /* not available in mysql client */
  {
    ocelot_opt_use_result= token2.toInt();
    return;
  }
  if ((token0_length >= sizeof("us")) && (strncmp(token0_as_utf8, "user", token0_length) == 0))
  {
    ocelot_user= token2;
    return;
  }
  /* todo: check that this finds both --vertical and -E */ /* for vertical */
  if (strcmp(token0_as_utf8, "verbose") == 0) { ocelot_verbose= 1; return; }
  if (strcmp(token0_as_utf8, "version") == 0) { ocelot_version= 1; return; }
  if (strcmp(token0_as_utf8, "vertical") == 0) { ocelot_result_grid_vertical= 1; return; }
  if ((token0_length >= sizeof("w")) && (strncmp(token0_as_utf8, "wait", token0_length) == 0))
  {
    ocelot_wait= 1;
    return;
  }
  if (strcmp(token0_as_utf8, "write_timeout") == 0)
  {
    ocelot_opt_write_timeout= token2.toInt();
    return;
  }
  if (strcmp(token0_as_utf8, "xml") == 0) { ocelot_xml= 1; return; }
}


/*
  Given the options that have actually been set to non-default values,
  form a CONNECT statement.
  todo: something should call this for the sake of history, at least.
  todo: instead o x > "", use if (QString::compare(x, " ") > 0)
*/
void MainWindow::connect_make_statement()
{
  QMessageBox msgBox;
  QString statement_text;

  statement_text= "CONNECT";
  if (ocelot_defaults_file > "") statement_text= statement_text + " defaults_file=" + ocelot_defaults_file;
  if (ocelot_no_defaults > 0) statement_text= statement_text + " no_defaults";
  if (ocelot_defaults_extra_file > "") statement_text= statement_text + " defaults_extra_file=" + ocelot_defaults_extra_file;
  if (ocelot_host > "") statement_text= statement_text + " host=" + ocelot_host;
  if (ocelot_user > "") statement_text= statement_text + " user=" + ocelot_user;
  if (ocelot_database > "") statement_text= statement_text + " database=" + ocelot_database;
  if (ocelot_port > 0) statement_text= statement_text + " port=" + QString::number(ocelot_port);
  if (ocelot_comments > 0) statement_text= statement_text + " comments";
  if (ocelot_init_command > "") statement_text= statement_text + " init_command=" + ocelot_init_command;
  if (ocelot_default_auth > "") statement_text= statement_text + " default_auth=" + ocelot_default_auth;
  if (ocelot_protocol > "") statement_text= statement_text + " protocol=" + ocelot_protocol;
  if (ocelot_password_was_specified > 0) statement_text= statement_text + " password=" + ocelot_password;
  if (ocelot_unix_socket > "") statement_text= statement_text + " socket=" + ocelot_unix_socket;
  if (ocelot_delimiter_str > "") statement_text= statement_text + " delimiter=" + ocelot_delimiter_str;
  if (ocelot_history_includes_warnings > 0) statement_text= statement_text + " show_warnings";
  if (ocelot_opt_connect_timeout > 0) statement_text= statement_text + " connect_timeout=" + QString::number(ocelot_opt_connect_timeout);
  if (ocelot_opt_compress > 0) statement_text= statement_text + " compress";
  if (ocelot_secure_auth > 0) statement_text= statement_text + " secure_auth";
  if (ocelot_opt_local_infile > 0) statement_text= statement_text + " local_infile";
  if (ocelot_safe_updates > 0) statement_text= statement_text + " safe_updates";
  if (ocelot_plugin_dir > "") statement_text= statement_text + " plugin_dir=" + ocelot_plugin_dir;
  if (ocelot_select_limit > 0) statement_text= statement_text + " select_limit=" + QString::number(ocelot_select_limit);
  if (ocelot_max_join_size > 0) statement_text= statement_text + " max_join_size=" + QString::number(ocelot_max_join_size);
  if (ocelot_silent > 0) statement_text= statement_text + " silent";
  if (ocelot_no_beep > 0) statement_text= statement_text + "no_beep";
  if (ocelot_wait > 0) statement_text= statement_text + "wait";
  if (ocelot_set_charset_name > "") statement_text= statement_text + "default_character_set=" + ocelot_set_charset_name;
  if (ocelot_opt_ssl > "") statement_text= statement_text + "ssl=" + ocelot_opt_ssl;
  if (ocelot_opt_ssl_ca > "") statement_text= statement_text + "ssl_ca=" + ocelot_opt_ssl_ca;
  if (ocelot_opt_ssl_capath > "") statement_text= statement_text + "ssl_capath=" + ocelot_opt_ssl_capath;
  if (ocelot_opt_ssl_cert > "") statement_text= statement_text + "ssl_cert=" + ocelot_opt_ssl_cert;
  if (ocelot_opt_ssl_cipher > "") statement_text= statement_text + "ssl_cipher=" + ocelot_opt_ssl_cipher;
  if (ocelot_opt_ssl_crl > "") statement_text= statement_text + "ssl_crl=" + ocelot_opt_ssl_crl;
  if (ocelot_opt_ssl_crlpath > "") statement_text= statement_text + "ssl_crlpath=" + ocelot_opt_ssl_crlpath;
  if (ocelot_opt_ssl_key > "") statement_text= statement_text + "ssl_key=" + ocelot_opt_ssl_key;
  if (ocelot_opt_ssl_verify_server_cert > 0) statement_text= statement_text + "ssl_verify_server_cert=" + ocelot_opt_ssl_verify_server_cert;
  msgBox.setText(statement_text);
  msgBox.exec();
}

#define PROTOCOL_TCP 1
#define PROTOCOL_SOCKET 2
#define PROTOCOL_PIPE 3
#define PROTOCOL_MEMORY 4
unsigned int MainWindow::get_ocelot_protocol_as_int(QString ocelot_protocol)
{
  if (QString::compare(ocelot_protocol, "TCP", Qt::CaseInsensitive) == 0) return PROTOCOL_TCP;
  if (QString::compare(ocelot_protocol, "SOCKET", Qt::CaseInsensitive) == 0) return PROTOCOL_SOCKET;
  if (QString::compare(ocelot_protocol, "PIPE", Qt::CaseInsensitive) == 0) return PROTOCOL_PIPE;
  if (QString::compare(ocelot_protocol, "MEMORY", Qt::CaseInsensitive) == 0) return PROTOCOL_MEMORY;
  return 0;
}

int options_and_connect(
    unsigned int connection_number)
{
  if (connected[connection_number] != 0)
  {
    connected[connection_number]= 0;
    mysql_close(&mysql[connection_number]);
  }
  mysql_init(&mysql[connection_number]);
#ifdef MYSQL_DEFAULT_AUTH
  if (ocelot_default_auth_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_DEFAULT_AUTH, ocelot_default_auth_as_utf8);
#endif
#ifdef MYSQL_ENABLE_CLEARTEXT_PLUGIN
  if (ocelot_enable_cleartext_plugin == true) mysql_options(&mysql[connection_number], MYSQL_ENABLE_CLEARTEXT_PLUGIN, ocelot_enable_cleartext_plugin);
#endif
  if (ocelot_init_command_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_INIT_COMMAND, ocelot_init_command_as_utf8);
#ifdef MYSQL_OPT_BIND
  if (ocelot_opt_bind_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_OPT_BIND, ocelot_opt_bind_as_utf8);
#endif
#ifdef MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS
  if (ocelot_can_handle_expired_passwords != 0) mysql_options(&mysql[connection_number], MYSQL_OPT_CAN_HANDLE_EXPIRED_PASSWORDS, ocelot_can_handle_expired_passwords);
#endif
  if (ocelot_opt_compress > 0) mysql_options(&mysql[connection_number], MYSQL_OPT_COMPRESS, NULL);
#ifdef MYSQL_OPT_CONNECT_ATTR_DELETE
  if (ocelot_opt_connect_attr_delete_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_OPT_CONNECT_ATTR_DELETE, ocelot_opt_connect_attr_delete_as_utf8);
#endif
#ifdef MYSQL_OPT_CONNECT_ATTR_RESET
  if (ocelot_opt_connect_attr_reset != 0) mysql_options(&mysql[connection_number], MYSQL_OPT_CONNECT_ATTR_RESET, (char*) &ocelot_opt_connect_attr_reset);
#endif
  if (ocelot_opt_connect_timeout != 0)
  {
    unsigned int timeout= ocelot_opt_connect_timeout;
    mysql_options(&mysql[connection_number], MYSQL_OPT_CONNECT_TIMEOUT, (char*) &timeout);
  }
  if (ocelot_opt_local_infile > 0) mysql_options(&mysql[connection_number], MYSQL_OPT_LOCAL_INFILE, (char*) &ocelot_opt_local_infile);
#ifdef MYSQL_OPT_NAMED_PIPE
  if (ocelot_opt_named_pipe > 0) mysql_options(&mysql[connection_number], MYSQL_OPT_NAMED_PIPE, (char*) &ocelot_opt_named_pipe);
#endif
  if (ocelot_protocol_as_int > 0) mysql_options(&mysql[connection_number], MYSQL_OPT_PROTOCOL, (char*)&ocelot_protocol_as_int);
#ifdef MYSQL_OPT_READ_TIMEOUT
  if (ocelot_opt_read_timeout > 0) mysql_options(&mysql[connection_number], MYSQL_OPT_READ_TIMEOUT, (char*)&ocelot_opt_read_timeout);
#endif
#ifdef MYSQL_OPT_RECONNECT
  if (ocelot_opt_reconnect > 0) mysql_options(&mysql[connection_number], MYSQL_OPT_RECONNECT, (char*)&ocelot_opt_reconnect);
#endif
#ifdef MYSQL_OPT_SSL_CA
  if (ocelot_opt_ssl_ca_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_OPT_SSL_CA, ocelot_ocelot_opt_ssl_ca_as_utf8);
#endif
#ifdef MYSQL_OPT_SSL_CAPATH
  if (ocelot_opt_ssl_capath_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_OPT_SSL_CAPATH, ocelot_ocelot_opt_ssl_capath_as_utf8);
#endif
#ifdef MYSQL_OPT_SSL_CERT
  if (ocelot_opt_ssl_cert_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_OPT_SSL_CERT, ocelot_ocelot_opt_ssl_cert_as_utf8);
#endif
#ifdef MYSQL_OPT_SSL_CIPHER
  if (ocelot_opt_ssl_cipher_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_OPT_SSL_CIPHER, ocelot_ocelot_opt_ssl_cipher_as_utf8);
#endif
#ifdef MYSQL_OPT_SSL_CRL
  if (ocelot_opt_ssl_crl_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_OPT_SSL_CRL, ocelot_ocelot_opt_ssl_crl_as_utf8);
#endif
#ifdef MYSQL_OPT_SSL_CRLPATH
  if (ocelot_opt_ssl_crlpath_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_OPT_SSL_CRLPATH, ocelot_ocelot_opt_ssl_crlpath_as_utf8);
#endif
#ifdef MYSQL_OPT_SSL_KEY
  if (ocelot_opt_ssl_key_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_OPT_SSL_key, ocelot_ocelot_opt_ssl_key_as_utf8);
#endif
#ifdef MYSQL_OPT_SSL_VERIFY_SERVER_CERT
  if (ocelot_opt_ssl_verify_server_cert > 0) mysql_options(&mysql[connection_number], MYSQL_OPT_SSL_VERIFY_SERVER_CERT, (char*) &ocelot_ocelot_opt_ssl_verify_server_cert);
#endif
#ifdef MYSQL_OPT_WRITE_TIMEOUT
  if (ocelot_opt_write_timeout > 0) mysql_options(&mysql[connection_number], MYSQL_OPT_WRITE_TIMEOUT, (char*)&ocelot_opt_write_timeout);
#endif
#ifdef MYSQL_PLUGIN_DIR
  if (ocelot_plugin_dir_as_utf8[0] != '\0') mysql_options(&mysql[], MYSQL_PLUGIN_DIR, ocelot_plugin_dir_as_utf8);
#endif
#ifdef MYSQL_READ_DEFAULT_FILE
  if (ocelot_read_default_file_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_READ_DEFAULT_FILE, ocelot_read_default_file_as_utf8);
#endif
#ifdef MYSQL_READ_DEFAULT_GROUP
  if (ocelot_read_default_group_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_READ_DEFAULT_GROUP, ocelot_read_default_group_as_utf8);
#endif
#ifdef MYSQL_REPORT_DATA_TRUNCATION
  if (ocelot_report_data_truncation > 0) mysql_options(&mysql[connection_number], MYSQL_REPORT_DATA_TRUNCATION, (char*) ocelot_report_data_truncation);
#endif
  if (ocelot_secure_auth > 0) mysql_options(&mysql[connection_number], MYSQL_SECURE_AUTH, (char *) &ocelot_secure_auth);
#ifdef MYSQL_SERVER_PUBLIC_KEY
  if (ocelot_server_public_key_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_SERVER_PUBLIC_KEY, (char *) ocelot_server_public_key_as_utf8);
#endif
#ifdef MYSQL_SET_CHARSET_DIR
  if (ocelot_set_charset_dir_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_SET_CHARSET_DIR, ocelot_set_charset_dir_as_utf8);
#endif
#ifdef MYSQL_SET_CHARSET_NAME
  if (ocelot_set_charset_name_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_SET_CHARSET_NAME, ocelot_set_charset_name_as_utf8);
#endif
#ifdef MYSQL_SHARED_MEMORY_BASE_NAME
  if (ocelot_shared_memory_base_name_as_utf8[0] != '\0') mysql_options(&mysql[connection_number], MYSQL_SHARED_MEMORY_BASE_NAME, ocelot_shared_memory_base_name_as_utf8);
#endif

  if (ocelot_safe_updates > 0)
  {
    char init_command[100]; /* todo: the size could be more dynamic here */
    sprintf(init_command,
        "SET sql_select_limit = %lu, sql_safe_updates = 1, max_join_size = %lu",
        ocelot_select_limit, ocelot_max_join_size);
    mysql_options(&mysql[connection_number], MYSQL_INIT_COMMAND, init_command);
  }

  /* CLIENT_MULTI_RESULTS but not CLIENT_MULTI_STATEMENTS */
  if (mysql_real_connect(&mysql[connection_number], ocelot_host_as_utf8, ocelot_user_as_utf8, ocelot_password_as_utf8,
                          ocelot_database_as_utf8, ocelot_port, ocelot_unix_socket_as_utf8,
                          CLIENT_MULTI_RESULTS) == 0)
  {
    /* connect failed. todo: better diagnostics? anyway, user can retry, a dialog box will come up. */
    return -1;					// Retryable
  }

  /*
    Tell the server: characters from the client are UTF8, characters going to the client are UTF8.
    This partially overrides --default_character_set, except there's no change to character_set_connection.
    Todo: Eventually UTF8MB4 will be preferable but check server version before trying that.
    Todo: We could have an option to receive in UTF16 which is what Qt would prefer.
    Todo: We should warn or disallow if user tries to change these.
    Todo: Think what we're going to do with file I/O, e.g. the user might expect it to be ujis.
    Todo: This makes the server do conversions. Eventually the client could do the conversions but that's lots of work.
    Todo: Worry that there might be some way to start Qt with different character-set assumptions.
    Todo: Worry that we haven't got a plan for _latin2 'string' etc. although we could get the server to translate for us
  */
  if (mysql_query(&mysql[connection_number], "set character_set_client = utf8")) printf("SET character_set_client failed\n");
  if (mysql_query(&mysql[connection_number], "set character_set_results = utf8")) printf("SET character_set_results failed\n");

  connected[connection_number]= 1;
  return 0;
}

/* Todo: check every one of the "new ..." results for failure. */
void MainWindow::copy_connect_strings_to_utf8()
{
  /* See comment "UTF8 Conversion" */
  if (ocelot_host_as_utf8 != 0) { delete [] ocelot_host_as_utf8; ocelot_host_as_utf8= 0; }
  int tmp_host_len= ocelot_host.toUtf8().size();
  ocelot_host_as_utf8= new char[tmp_host_len + 1];
  memcpy(ocelot_host_as_utf8, ocelot_host.toUtf8().constData(), tmp_host_len + 1);

  if (ocelot_database_as_utf8 != 0) { delete [] ocelot_database_as_utf8; ocelot_database_as_utf8= 0; }
  int tmp_database_len= ocelot_database.toUtf8().size();
  ocelot_database_as_utf8= new char[tmp_database_len + 1];
  memcpy(ocelot_database_as_utf8, ocelot_database.toUtf8().constData(), tmp_database_len + 1);

  if (ocelot_user_as_utf8 != 0) { delete [] ocelot_user_as_utf8; ocelot_user_as_utf8= 0; }
  int tmp_user_len= ocelot_user.toUtf8().size();
  ocelot_user_as_utf8= new char[tmp_user_len + 1];
  memcpy(ocelot_user_as_utf8, ocelot_user.toUtf8().constData(), tmp_user_len + 1);

  if (ocelot_password_as_utf8 != 0) { delete [] ocelot_password_as_utf8; ocelot_password_as_utf8= 0; }
  int tmp_password_len= ocelot_password.toUtf8().size();
  ocelot_password_as_utf8= new char[tmp_password_len + 1];
  memcpy(ocelot_password_as_utf8, ocelot_password.toUtf8().constData(), tmp_password_len + 1);

  if (ocelot_default_auth_as_utf8 != 0) { delete [] ocelot_default_auth_as_utf8; ocelot_default_auth_as_utf8= 0; }
  int tmp_default_auth_len= ocelot_default_auth.toUtf8().size();
  ocelot_default_auth_as_utf8= new char[tmp_default_auth_len + 1];
  memcpy(ocelot_default_auth_as_utf8, ocelot_default_auth.toUtf8().constData(), tmp_default_auth_len + 1);

  if (ocelot_init_command_as_utf8 != 0) { delete [] ocelot_init_command_as_utf8; ocelot_init_command_as_utf8= 0; }
  int tmp_init_command_len= ocelot_init_command.toUtf8().size();
  ocelot_init_command_as_utf8= new char[tmp_init_command_len + 1];
  memcpy(ocelot_init_command_as_utf8, ocelot_init_command.toUtf8().constData(), tmp_init_command_len + 1);

  if (ocelot_opt_bind_as_utf8 != 0) { delete [] ocelot_opt_bind_as_utf8; ocelot_opt_bind_as_utf8= 0; }
  int tmp_opt_bind_len= ocelot_opt_bind.toUtf8().size();
  ocelot_opt_bind_as_utf8= new char[tmp_opt_bind_len + 1];
  memcpy(ocelot_opt_bind_as_utf8, ocelot_opt_bind.toUtf8().constData(), tmp_opt_bind_len + 1);

  if (ocelot_opt_connect_attr_delete_as_utf8 != 0) { delete [] ocelot_opt_connect_attr_delete_as_utf8; ocelot_opt_connect_attr_delete_as_utf8= 0; }
  int tmp_opt_connect_attr_delete_len= ocelot_opt_connect_attr_delete.toUtf8().size();
  ocelot_opt_connect_attr_delete_as_utf8= new char[tmp_opt_connect_attr_delete_len + 1];
  memcpy(ocelot_opt_connect_attr_delete_as_utf8, ocelot_opt_connect_attr_delete.toUtf8().constData(), tmp_opt_connect_attr_delete_len + 1);

  if (ocelot_opt_ssl_as_utf8 != 0) { delete [] ocelot_opt_ssl_as_utf8; ocelot_opt_ssl_as_utf8= 0; }
  int tmp_opt_ssl_len= ocelot_opt_ssl.toUtf8().size();
  ocelot_opt_ssl_as_utf8= new char[tmp_opt_ssl_len + 1];
  memcpy(ocelot_opt_ssl_as_utf8, ocelot_opt_ssl.toUtf8().constData(), tmp_opt_ssl_len + 1);

  if (ocelot_opt_ssl_ca_as_utf8 != 0) { delete [] ocelot_opt_ssl_ca_as_utf8; ocelot_opt_ssl_ca_as_utf8= 0; }
  int tmp_opt_ssl_ca_len= ocelot_opt_ssl_ca.toUtf8().size();
  ocelot_opt_ssl_ca_as_utf8= new char[tmp_opt_ssl_ca_len + 1];
  memcpy(ocelot_opt_ssl_ca_as_utf8, ocelot_opt_ssl_ca.toUtf8().constData(), tmp_opt_ssl_ca_len + 1);

  if (ocelot_opt_ssl_capath_as_utf8 != 0) { delete [] ocelot_opt_ssl_capath_as_utf8; ocelot_opt_ssl_capath_as_utf8= 0; }
  int tmp_opt_ssl_capath_len= ocelot_opt_ssl_capath.toUtf8().size();
  ocelot_opt_ssl_capath_as_utf8= new char[tmp_opt_ssl_capath_len + 1];
  memcpy(ocelot_opt_ssl_capath_as_utf8, ocelot_opt_ssl_capath.toUtf8().constData(), tmp_opt_ssl_capath_len + 1);

  if (ocelot_opt_ssl_cert_as_utf8 != 0) { delete [] ocelot_opt_ssl_cert_as_utf8; ocelot_opt_ssl_cert_as_utf8= 0; }
  int tmp_opt_ssl_cert_len= ocelot_opt_ssl_cert.toUtf8().size();
  ocelot_opt_ssl_cert_as_utf8= new char[tmp_opt_ssl_cert_len + 1];
  memcpy(ocelot_opt_ssl_cert_as_utf8, ocelot_opt_ssl_cert.toUtf8().constData(), tmp_opt_ssl_cert_len + 1);

  if (ocelot_opt_ssl_cipher_as_utf8 != 0) { delete [] ocelot_opt_ssl_cipher_as_utf8; ocelot_opt_ssl_cipher_as_utf8= 0; }
  int tmp_opt_ssl_cipher_len= ocelot_opt_ssl_cipher.toUtf8().size();
  ocelot_opt_ssl_cipher_as_utf8= new char[tmp_opt_ssl_cipher_len + 1];
  memcpy(ocelot_opt_ssl_cipher_as_utf8, ocelot_opt_ssl_cipher.toUtf8().constData(), tmp_opt_ssl_cipher_len + 1);

  if (ocelot_opt_ssl_crl_as_utf8 != 0) { delete [] ocelot_opt_ssl_crl_as_utf8; ocelot_opt_ssl_crl_as_utf8= 0; }
  int tmp_opt_ssl_crl_len= ocelot_opt_ssl_crl.toUtf8().size();
  ocelot_opt_ssl_crl_as_utf8= new char[tmp_opt_ssl_crl_len + 1];
  memcpy(ocelot_opt_ssl_crl_as_utf8, ocelot_opt_ssl_crl.toUtf8().constData(), tmp_opt_ssl_crl_len + 1);

  if (ocelot_opt_ssl_crlpath_as_utf8 != 0) { delete [] ocelot_opt_ssl_crlpath_as_utf8; ocelot_opt_ssl_crlpath_as_utf8= 0; }
  int tmp_opt_ssl_crlpath_len= ocelot_opt_ssl_crlpath.toUtf8().size();
  ocelot_opt_ssl_crlpath_as_utf8= new char[tmp_opt_ssl_crlpath_len + 1];
  memcpy(ocelot_opt_ssl_crlpath_as_utf8, ocelot_opt_ssl_crlpath.toUtf8().constData(), tmp_opt_ssl_crlpath_len + 1);

  if (ocelot_opt_ssl_key_as_utf8 != 0) { delete [] ocelot_opt_ssl_key_as_utf8; ocelot_opt_ssl_key_as_utf8= 0; }
  int tmp_opt_ssl_key_len= ocelot_opt_ssl_key.toUtf8().size();
  ocelot_opt_ssl_key_as_utf8= new char[tmp_opt_ssl_key_len + 1];
  memcpy(ocelot_opt_ssl_key_as_utf8, ocelot_opt_ssl_key.toUtf8().constData(), tmp_opt_ssl_key_len + 1);

  if (ocelot_plugin_dir_as_utf8 != 0) { delete [] ocelot_plugin_dir_as_utf8; ocelot_plugin_dir_as_utf8= 0; }
  int tmp_plugin_dir_len= ocelot_plugin_dir.toUtf8().size();
  ocelot_plugin_dir_as_utf8= new char[tmp_plugin_dir_len + 1];
  memcpy(ocelot_plugin_dir_as_utf8, ocelot_plugin_dir.toUtf8().constData(), tmp_plugin_dir_len + 1);

  if (ocelot_read_default_file_as_utf8 != 0) { delete [] ocelot_read_default_file_as_utf8; ocelot_read_default_file_as_utf8= 0; }
  int tmp_read_default_file_len= ocelot_read_default_file.toUtf8().size();
  ocelot_read_default_file_as_utf8= new char[tmp_read_default_file_len + 1];
  memcpy(ocelot_read_default_file_as_utf8, ocelot_read_default_file.toUtf8().constData(), tmp_read_default_file_len + 1);

  if (ocelot_read_default_group_as_utf8 != 0) { delete [] ocelot_read_default_group_as_utf8; ocelot_read_default_group_as_utf8= 0; }
  int tmp_read_default_group_len= ocelot_read_default_group.toUtf8().size();
  ocelot_read_default_group_as_utf8= new char[tmp_read_default_group_len + 1];
  memcpy(ocelot_read_default_group_as_utf8, ocelot_read_default_group.toUtf8().constData(), tmp_read_default_group_len + 1);

  if (ocelot_server_public_key_as_utf8 != 0) { delete [] ocelot_server_public_key_as_utf8; ocelot_server_public_key_as_utf8= 0; }
  int tmp_server_public_key_len= ocelot_server_public_key.toUtf8().size();
  ocelot_server_public_key_as_utf8= new char[tmp_server_public_key_len + 1];
  memcpy(ocelot_server_public_key_as_utf8, ocelot_server_public_key.toUtf8().constData(), tmp_server_public_key_len + 1);

  if (ocelot_unix_socket_as_utf8 != 0) { delete [] ocelot_unix_socket_as_utf8; ocelot_unix_socket_as_utf8= 0; }
  int tmp_unix_socket_len= ocelot_unix_socket.toUtf8().size();
  ocelot_unix_socket_as_utf8= new char[tmp_unix_socket_len + 1];
  memcpy(ocelot_unix_socket_as_utf8, ocelot_unix_socket.toUtf8().constData(), tmp_unix_socket_len + 1);

  if (ocelot_set_charset_dir_as_utf8 != 0) { delete [] ocelot_set_charset_dir_as_utf8; ocelot_set_charset_dir_as_utf8= 0; }
  int tmp_set_charset_dir_len= ocelot_set_charset_dir.toUtf8().size();
  ocelot_set_charset_dir_as_utf8= new char[tmp_set_charset_dir_len + 1];
  memcpy(ocelot_set_charset_dir_as_utf8, ocelot_set_charset_dir.toUtf8().constData(), tmp_set_charset_dir_len + 1);

  if (ocelot_set_charset_name_as_utf8 != 0) { delete [] ocelot_set_charset_name_as_utf8; ocelot_set_charset_name_as_utf8= 0; }
  int tmp_set_charset_name_len= ocelot_set_charset_name.toUtf8().size();
  ocelot_set_charset_name_as_utf8= new char[tmp_set_charset_name_len + 1];
  memcpy(ocelot_set_charset_name_as_utf8, ocelot_set_charset_name.toUtf8().constData(), tmp_set_charset_name_len + 1);

  if (ocelot_shared_memory_base_name_as_utf8 != 0) { delete [] ocelot_shared_memory_base_name_as_utf8; ocelot_shared_memory_base_name_as_utf8= 0; }
  int tmp_shared_memory_base_name_len= ocelot_shared_memory_base_name.toUtf8().size();
  ocelot_shared_memory_base_name_as_utf8= new char[tmp_shared_memory_base_name_len + 1];
  memcpy(ocelot_shared_memory_base_name_as_utf8, ocelot_shared_memory_base_name.toUtf8().constData(), tmp_shared_memory_base_name_len + 1);
}


int MainWindow::the_connect(unsigned int connection_number)
{
  int x;

  /* options_and_connect() cannot use QStrings because it is not in MainWindow */
  copy_connect_strings_to_utf8();

  x= options_and_connect(connection_number);

  return x;
}
