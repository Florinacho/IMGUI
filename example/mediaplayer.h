#ifndef __DEMO_MEDIAPLAYER_H__
#define __DEMO_MEDIAPLAYER_H__

#include <imgui.h>

static ivec4 mediaPlayerBounds = {100, 100, 800, 600};
static uint32_t mediaPlayerFlags = GUI_FLAGS_WINDOW | GUI_WINDOW_SIZE;
static char mediaPlayerTitle[32] = "Media Player";

void mediaPlayerPanel() {
	static const char* LIBRARY_BUTTON_LABELS[] = {
		"Open",
		"Home",
		"Music",
		"Video",
		"Queue",
		"Playlists",
	};
	static const char* PLAY_BUTTON_LABELS[] = {
		">",
		"||",
	};
	static bool playing = false;
	static uint32_t rawProgress = 0;
	static float progress = 0.0f;
	static int speed = 1;
	static float volume = 0.5f;

	static char labelText[32];

	const ivec4 bounds = LayoutGetAbsoluteBounds(false);
	const uint32_t width = bounds.z - bounds.x;

	if (playing) {
		snprintf(labelText, sizeof(labelText), "Playing %s at %dx ... %d%%", "test.mp4", speed, int(progress * 100.0f));
	} else {
		snprintf(labelText, sizeof(labelText), "Paused");
	}

	Panel(SplitLayout(GUI_VERTICAL, 0.85, 0)) {
		static float proc = 0.2f;

		if (width > 450 ) {
			SplitPanel(GUI_HORIZONTAL, proc) {
				static int xProg = 0;
				ScrollPanel(100, 200, NULL, &xProg) {
					Panel(FixSplitLayout(GUI_VERTICAL, 200)) {
						Panel(GridLayout(1, 6)) {
							for (auto label : LIBRARY_BUTTON_LABELS) {
								if (Button(label)) {
									printf("%s\n", label);
								}
							}
						}
					}
				}
				Label(labelText);
			}
		} else {
			Label(labelText);
		}

		Panel(FixSplitLayout(GUI_VERTICAL, 25, 0)) {
			progress = (double)rawProgress / (double)0x0000FFFF;
			Panel(BorderLayout(GUI_HORIZONTAL, 0.04f, 0.04f)) {
				if (Button("<<")) {
					speed = std::max(speed - 1, -5);
				}
				if (Slider(progress)) {
					rawProgress = progress * 0x0000FFFF;
				}
				if (Button(">>")) {
					speed = std::min(speed + 1, 5);
				}
			}
			Panel(BorderLayout(GUI_HORIZONTAL, 0.35f, 0.35f)) {
				Panel(SplitLayout(GUI_VERTICAL)) {
					Label("Volume");
					Slider(volume);
				}
				Panel(GridLayout(3, 1)) {
					if (Button("|<")) {
						rawProgress = 0;
					}
					if (Button(PLAY_BUTTON_LABELS[playing])) {
						playing = !playing;
					}
					if (Button(">|")) {
						rawProgress = 0;
					}
				}
				DummyElement();
			}
		}
	}
	rawProgress = std::clamp<uint32_t>(rawProgress + playing * speed, 0, 0x0000FFFF);
}

void mediaPlayerWindow() {
	Window(&mediaPlayerBounds, mediaPlayerTitle, 2, &mediaPlayerFlags) {
		mediaPlayerPanel();
	}
}

#endif // #ifndef __DEMO_MEDIAPLAYER_H__
