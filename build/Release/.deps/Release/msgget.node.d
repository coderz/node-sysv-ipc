cmd_Release/msgget.node := ln -f "Release/obj.target/msgget.node" "Release/msgget.node" 2>/dev/null || (rm -rf "Release/msgget.node" && cp -af "Release/obj.target/msgget.node" "Release/msgget.node")
