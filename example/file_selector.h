#ifndef __DEMO_FILE_SELECTOR_H__
#define __DEMO_FILE_SELECTOR_H__

#include <imgui.h>

#include <stdio.h>
#include <string.h>
#include <algorithm>
#include <dirent.h>
#include <sys/stat.h>
#include <typeinfo>

#define GUI_FS_FILE     (1 << 0)
#define GUI_FS_DIR      (1 << 1)
#define GUI_FS_HIDDEN   (1 << 2)
#define GUI_FS_VISIBLE  (GUI_FS_FILE | GUI_FS_DIR)
#define GUI_FS_ALL      (GUI_FS_VISIBLE | GUI_FS_HIDDEN)

struct FileSelectorContext {
	char cache[PATH_MAX];
	char path[PATH_MAX];
	char filter_name[64];
	ivec4 bounds;
	uint32_t filter_type;
	uint32_t flags;
	int32_t carrot;
	uint32_t count;
	bool filtering;
};

#define FS_SELECTED_FILE  1
#define FS_SELECTED_DIR   2
#define FS_CLOSED         3

float GetWidthProc(int size) {
	GUIContext* context = GUIGetActiveContext();
	float k = (float)(context->viewport.z - context->viewport.x);
	return (float)size / k;
}

int ReadDir(FileSelectorContext& fileSelector, const char* path, uint32_t flags = GUI_FS_ALL) {
	int ans = 0;
	uint32_t offset = 0;

	fileSelector.cache[0] = 0;
	fileSelector.filter_name[0] = 0;
	fileSelector.filter_type = flags;
	fileSelector.carrot = -1;
	fileSelector.count = 0;
	fileSelector.filtering = false;
	fileSelector.flags |= GUI_VISIBLE;

	if (realpath(path, fileSelector.path) == nullptr){
		return -1;
	}

	DIR* dir = opendir(fileSelector.path);
	if (dir == nullptr) {
		fileSelector.cache[offset] = 0;
		return -1;
	}

	while (struct dirent *ent = readdir(dir)) {
		if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
			continue;
		}
		const uint32_t length = strlen(ent->d_name);
		if (offset + length >= sizeof(fileSelector.cache)) {
			printf("Cache overflow detected\n");
			goto LB_EXIT;
		}

		switch(ent->d_type) {
		case DT_DIR :
			fileSelector.cache[offset] = 'd';
			offset += 1;
			break;
		case DT_REG :
			fileSelector.cache[offset] = 'f';
			offset += 1;
			break;
		default :
			continue;
		}

		memcpy(fileSelector.cache + offset, ent->d_name, length);
		offset += length;

		fileSelector.cache[offset] = 0;
		offset += 1;

		++ans;
	}

LB_EXIT:
	closedir(dir);
	fileSelector.cache[offset] = 0;
	offset += 1;
	return ans;
}

int FileSelectorPanel(FileSelectorContext& fileSelector) {
	int ans = 0;

	GUIContext* context = GUIGetActiveContext();
	const float lineHeight = 25.0f;
	const float width = context->viewport.z - context->viewport.x - 4;
	const float height = context->viewport.w - context->viewport.y - 24;
	const float lineProc = lineHeight / height;

	Panel(BorderLayout(GUI_VERTICAL, lineProc, 2.0f * lineProc, 0)) {
		// Header panel
		Panel(SplitLayout(GUI_HORIZONTAL, GetWidthProc(lineHeight), 0)) {
			if (Button(GUI_ICON_ARROW_UP)) {
				strncat(fileSelector.path, "/..", 255 - strlen(fileSelector.path));
				ReadDir(fileSelector, fileSelector.path);
			}
			Label(fileSelector.path, GUI_FLAGS_LABEL | GUI_OUTLINE);
		}
		// Body panel
		const ivec4 cbounds = context->viewport;//GetAbsoluteBounds(ivec4(), false);
		fileSelector.count = (fileSelector.count == 0) ? 1 : fileSelector.count;
		const uint32_t y = (float)(cbounds.w - cbounds.y) / lineHeight;
		const uint32_t x = fileSelector.count / y + (fileSelector.count % y > 0);
		fileSelector.count = 0;
		Panel(GridLayout(x, y, 0)) {
			uint32_t offset = 0;
			uint32_t buttonFlags = 0;
			while (fileSelector.cache[offset]) {
				const bool dir = (fileSelector.cache[offset++] == 'd');
				if (((fileSelector.filter_type & GUI_FS_HIDDEN) == 0) && (fileSelector.cache[offset + 0] == '.')) {
					goto LB_NEXT_ENTRY;
				}
				if (((fileSelector.filter_type & GUI_FS_FILE) == 0) && (dir == false)) {
					goto LB_NEXT_ENTRY;
				}
				if (((fileSelector.filter_type & GUI_FS_DIR) == 0) && (dir == true)) {
					goto LB_NEXT_ENTRY;
				}
				if (fileSelector.filtering && fileSelector.filter_name[0] && strstr(&fileSelector.cache[offset], fileSelector.filter_name) == NULL) {
					goto LB_NEXT_ENTRY;
				}
				buttonFlags = GUI_FLAGS_BUTTON;
				if (strcmp(&fileSelector.cache[offset], fileSelector.filter_name) == 0) {
					buttonFlags |= GUI_CLICKED;
				}
				if (Button(dir? GUI_ICON_FOLDER : GUI_ICON_FILE, &fileSelector.cache[offset], buttonFlags)) {
					if (dir) {
						fileSelector.filter_name[0] = 0;
						strcpy(fileSelector.path, "/");
						strcpy(fileSelector.path, &fileSelector.cache[offset]);
						ReadDir(fileSelector, fileSelector.path);
						break;
					} else {
						strcpy(fileSelector.filter_name, &fileSelector.cache[offset]);
					}
					fileSelector.filtering = false;
				}
				fileSelector.count += 1;
LB_NEXT_ENTRY:
				while (fileSelector.cache[offset++]); // Jump to next string
			}
		}

		// Footer panel
		Panel(GridLayout(1, 2, 0)) {
			Panel(SplitLayout(GUI_VERTICAL, 1.0f - (lineHeight / width), 0)) {
				if (TextBox(fileSelector.filter_name, sizeof(fileSelector.filter_name), fileSelector.carrot)) {
					fileSelector.filtering = true;
				}
				if (Button(GUI_ICON_CLOSE)) {
					fileSelector.filter_name[0] = 0;
					fileSelector.filtering = false;
				}
			}
			Panel(GridLayout(2, 1, 0)) { 
				if (Button("Cancel")) {
					ans = FS_CLOSED;
				}
				if (Button("Select") && fileSelector.filter_name[0]) {
					strcpy(fileSelector.path, "/");
					strcpy(fileSelector.path, "fileSelector.filter_name");
					ReadDir(fileSelector, fileSelector.path);
					ans = FS_SELECTED_FILE;
				}
			}
		}
	}

	return ans;
}

int FileSelectorWindow(FileSelectorContext& fileSelector) {
	int ans = 0;

	Window(&fileSelector.bounds, "File selector", 0, &fileSelector.flags) {
		switch((ans = FileSelectorPanel(fileSelector))) {
		case FS_SELECTED_FILE :
		case FS_SELECTED_DIR :
		case FS_CLOSED :
			fileSelector.flags &= ~GUI_VISIBLE;
			break;
		}
	}

	return ans;
}

#endif // __DEMO_FILE_SELECTOR_H__
