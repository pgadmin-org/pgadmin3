Class naming conventions
=========================

foo represents an object of type Foo
fooFactory creates foos
fooCollection is a collection of foos
fooCollectionFactory creates a fooCollection

fooObject is an object that resides under/in foo
fooObjCollection is a collection of fooObjects

fooObjFactory creates fooObjects



To add actions to the main window (frmMain):
=============================================

- create the frmXXX class that will render the new function.
- create an actionFactory (or contextActionFactory, if the action will display in context menus too) derived factory for each action to be performed, and register it in frmMain::CreateMenus(). Never touch menu.h, or anything for that. Easy examples are frmGrantWizard or frmEditGrid.

   - Implement CheckEnable() for each factory, checking the current object if the action applies.
   - Implement StartDialog(), which brings up the dialog.

Following this, you should be able to add new actions on objects by just adding the source and registering the new menu in frmMain.cpp. If you think you need to modify more files, you're probably wrong.




To add objects to the object tree:
===================================
- Implement the class with proper hierarchy. pgCast and dlgCast are simple examples. 
- instantiate a collection in the pgXXX object where it should be located under.

