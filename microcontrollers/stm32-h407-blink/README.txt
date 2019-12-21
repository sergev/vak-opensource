
	Project STM32-H407_blink_FLASH README

        Simple blinking LED example for the STM32-H107 board configured to run in FLASH

	|Last edited: 05.10.2012
					by OLIMEX

	I.  Important Notes
		Current project:

		- always pause the debugger before stopping it completely

		General:
		- Always restart your programmer and openOCD
		  when starting work with a new project so that
		  the new configuration (starting from project.cfg)
		  can be used for OpenOCD

		- Always have one source file from a project
		  opened and active in the editor before launching
		  an external tool or the debugger; if there is no source
		  file on focus in the editor, the ext.tools and debugger
		  can't figure out the project's root path and you
		  will get an error that has to do with an unknown
		  (proj_dir) path.

		- The project has already been built, if you want
		  to use it without changes, just launch your
		  external tool(it launches OpenOCD with the correct
		  programmer configuration + project configuration)
		  and start debugging.

		- If you want to create a new project for your board
		  and keep all the configurations, copy and paste the
		  project right within the workspace palette; this will
		  create a new project named "Copy of <project_name>"
		  with the same properties. Next, you should create
		  a new debug configuration for your project, name it
		  after it and copy all the settings from the original
		  project's debug configuretion; you can actually use
		  the old configuration for the new project, but it's
		  better practice to keep them separate.

		- If you want to copy and paste a project from within
		  the workspace palette you shoul open it first.

		- If you want to see the debug configurations of currently
		  closed projects, click "Filter debug configurations..."
		  and choose the settings that best suit your workflow.

		- If you want a debug configuration to appear in the
		  favorites menu below the debug button, tick the
		  checkbox in the "Common" tab of the Debug Configurations
		  window.

		- Every project has a project.cfg file, which is always
		  called when launching openOCD; it includes all the
		  project specific .cfg files


	II. How to contact:
		If you have any questions, email
		support@olimex.com

		OLIMEX, 2012
		http://www.olimex.com
		https://www.olimex.com/forum/
