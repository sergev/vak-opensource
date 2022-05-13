clean:
		rm -f *~

talking-clock:  talking-clock.py
		pyinstaller --onefile talking-clock.py
		mv dist/talking-clock $@
		rm -rf build dist talking-clock.spec
