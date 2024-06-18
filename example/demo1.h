#include <imgui.h>

void demo1Panel() {
	auto headerPanel = []() {
		Label("Header", GUI_FLAGS_LABEL | GUI_BACKGROUND | GUI_OUTLINE);
	};

	auto bodyPanel = []() {
		auto contentLeftPanel = []() {
			static int oy = 0;
			ScrollPanel(0, 10 * 30, nullptr, &oy) {
				SetLayout(GridLayout(1, 10, 0));
				for (uint32_t index = 0; index < 10; ++index) {
					Button("Test");
				}
			}
		};

		auto contentCenterPanel = []() {
			static int obx = 200 - 32, oby = 0;
			ScrollPanel(800, 400, &obx, &oby) {
				SetLayout(GridLayout(1, 10));
				Label("! @ # $ % ^ & * ( ) _ + ");
				Label("1 2 3 4 5 6 7 8 9 0");
				Label("a b c d e f g h i j k l m n o p q r s t u v w x y z");
				Label("` ~ [ ] \\ ; ' , . / { } | : \" < > ? ");
				Label("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z");
				Label("! @ # $ % ^ & * ( ) _ + ");
				Label("1 2 3 4 5 6 7 8 9 0");
				Label("a b c d e f g h i j k l m n o p q r s t u v w x y z");
				Label("` ~ [ ] \\ ; ' , . / { } | : \" < > ? ");
				Label("A B C D E F G H I J K L M N O P Q R S T U V W X Y Z");
			}
		};

		auto contentRightPanel = []() {
			Label("Right", GUI_FLAGS_LABEL | GUI_BACKGROUND | GUI_OUTLINE);
		};
		
		Panel(BorderLayout(GUI_HORIZONTAL, 0.2f, 0.2f)) {
			contentLeftPanel();
			contentCenterPanel();
			contentRightPanel();
		}
	};

	auto footerPanel = []() {
		Label("Footer", GUI_FLAGS_LABEL | GUI_BACKGROUND | GUI_OUTLINE);
	};

	Panel(BorderLayout(GUI_VERTICAL, 0.15f, 0.15f)) {
		headerPanel();
		bodyPanel();
		footerPanel();
	}
}
