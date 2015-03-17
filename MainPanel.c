/*
htop - ColumnsPanel.c
(C) 2004-2015 Hisham H. Muhammad
Released under the GNU GPL, see the COPYING file
in the source distribution for its full text.
*/

#include "MainPanel.h"
#include "Process.h"
#include "Platform.h"
#include "CRT.h"

#include <stdlib.h>

/*{
#include "Panel.h"
#include "Action.h"
#include "Settings.h"

typedef struct MainPanel_ {
   Panel super;
   State* state;
   FunctionBar* fuBar;
   Htop_Action *keys;
   pid_t pidSearch;
} MainPanel;

typedef bool(*MainPanel_ForeachProcessFn)(Process*, size_t);

}*/

void MainPanel_updateTreeFunctions(FunctionBar* fuBar, bool mode) {
   if (mode) {
      FunctionBar_setLabel(fuBar, KEY_F(5), "Sorted");
      FunctionBar_setLabel(fuBar, KEY_F(6), "Collap");
   } else {
      FunctionBar_setLabel(fuBar, KEY_F(5), "Tree  ");
      FunctionBar_setLabel(fuBar, KEY_F(6), "SortBy");
   }
}

void MainPanel_pidSearch(MainPanel* this, int ch) {
   Panel* super = (Panel*) this;
   pid_t pid = ch-48 + this->pidSearch;
   for (int i = 0; i < Panel_size(super); i++) {
      Process* p = (Process*) Panel_get(super, i);
      if (p && p->pid == pid) {
         Panel_setSelected(super, i);
         break;
      }
   }
   this->pidSearch = pid * 10;
   if (this->pidSearch > 10000000) {
      this->pidSearch = 0;
   }
}

static HandlerResult MainPanel_eventHandler(Panel* super, int ch) {
   MainPanel* this = (MainPanel*) super;

   HandlerResult result = IGNORED;
   
   Htop_Reaction reaction = HTOP_OK;

   if (ch == 27) {
      return HANDLED;
   }
   if(ch != ERR && this->keys[ch]) {
      reaction |= (this->keys[ch])(this->state);
      result = HANDLED;
   } else if (isdigit(ch)) {
      MainPanel_pidSearch(this, ch);
   } else {
      if (ch != ERR) {
         this->pidSearch = 0;
      }
      switch (ch) {
      case KEY_LEFT:
      case KEY_CTRLB:
         if (super->scrollH > 0) {
            super->scrollH -= CRT_scrollHAmount;
            super->needsRedraw = true;
         }
         return HANDLED;
      case KEY_RIGHT:
      case KEY_CTRLF:
         super->scrollH += CRT_scrollHAmount;
         super->needsRedraw = true;
         return HANDLED;
      }
   }

   if (reaction & HTOP_REDRAW_BAR) {
      MainPanel_updateTreeFunctions(this->fuBar, this->state->settings->treeView);
      IncSet_drawBar(this->state->inc);
   }
   if (reaction & HTOP_UPDATE_PANELHDR) {
      ProcessList_printHeader(this->state->pl, Panel_getHeader(super));
   }
   if (reaction & HTOP_REFRESH) {
      result |= REFRESH;
   }      
   if (reaction & HTOP_RECALCULATE) {
      result |= RECALCULATE;
   }
   if (reaction & HTOP_SAVE_SETTINGS) {
      this->state->settings->changed = true;
   }
   if (reaction & HTOP_QUIT) {
      return BREAK_LOOP;
   }
   if (!(reaction & HTOP_KEEP_FOLLOWING)) {
      this->state->pl->following = -1;
   }
   return result;
}

int MainPanel_selectedPid(MainPanel* this) {
   Process* p = (Process*) Panel_getSelected((Panel*)this);
   if (p) {
      return p->pid;
   }
   return -1;
}

const char* MainPanel_getValue(MainPanel* this, int i) {
   Process* p = (Process*) Panel_get((Panel*)this, i);
   if (p)
      return p->comm;
   return "";
}

bool MainPanel_foreachProcess(MainPanel* this, MainPanel_ForeachProcessFn fn, int arg, bool* wasAnyTagged) {
   Panel* super = (Panel*) this;
   bool ok = true;
   bool anyTagged = false;
   for (int i = 0; i < Panel_size(super); i++) {
      Process* p = (Process*) Panel_get(super, i);
      if (p->tag) {
         ok = fn(p, arg) && ok;
         anyTagged = true;
      }
   }
   if (!anyTagged) {
      Process* p = (Process*) Panel_getSelected(super);
      if (p) ok = fn(p, arg) && ok;
   }
   if (wasAnyTagged)
      *wasAnyTagged = anyTagged;
   return ok;
}

PanelClass MainPanel_class = {
   .super = {
      .extends = Class(Panel),
      .delete = MainPanel_delete
   },
   .eventHandler = MainPanel_eventHandler
};

MainPanel* MainPanel_new(FunctionBar* fuBar) {
   MainPanel* this = AllocThis(MainPanel);
   Panel_init((Panel*) this, 1, 1, 1, 1, Class(Process), false);
   this->keys = calloc(KEY_MAX, sizeof(Htop_Action));
   this->fuBar = fuBar;

   Action_setBindings(this->keys);
   Platform_setBindings(this->keys);

   return this;
}

void MainPanel_setState(MainPanel* this, State* state) {
   this->state = state;
}

void MainPanel_delete(Object* object) {
   Panel* super = (Panel*) object;
   MainPanel* this = (MainPanel*) object;
   Panel_done(super);
   free(this->keys);
   free(this);
}