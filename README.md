# Procedural Modeling Systerm
## Currently it is only a NodeGraphAsset plugin with Houdini Node UI in Unreal（目前它只是个UE中的有着Houdini UI的节点图资产）

### 1.Functionality（已有功能）
- 1.1 New Asset type（新的资产类型）

![image](https://user-images.githubusercontent.com/73771239/163821687-564e56f5-19ea-4af9-ab6b-57df5223bb6a.png)
- 1.2 Asset Editor Layout（资产编辑器所需要的Tab和布局）

![image](https://user-images.githubusercontent.com/73771239/163821972-cf0917a9-4a4e-42fa-b1af-b5ee8c0d8f5f.png)
- 1.3 Houdini ui node and connection（Houdini ui 节点与连线）

![image](https://user-images.githubusercontent.com/73771239/163822183-846586e9-9707-4060-9cde-a2c717389ee8.png)
- 1.4 Snap and align when dragging node（自动吸附对齐已经节点）

![image](https://user-images.githubusercontent.com/73771239/163822331-eb54935a-60ec-48a6-930a-e7dcec546995.png)
- 1.5 Support Ctrl/Shift drag in houdini（Ctrl/Shift拖动节点）

![image](https://user-images.githubusercontent.com/73771239/163822393-c660bb0f-0b7d-4e8f-9efd-3f1b9af1fc96.png)
y- 1.6 Draw cutting curve in graph panel（在节点图中画切割线，但目前没有实现切割）

![image](https://user-images.githubusercontent.com/73771239/163822505-641858d7-4fad-4e62-bcfd-b081e2a3ab56.png)
- 1.7 Nodes Menu（非蓝图式的节点菜单）

![image](https://user-images.githubusercontent.com/73771239/163822548-25e81dc0-5068-430c-8ec0-95d4b9852e4e.png)
- 1.8 Subnet（子图）

Unable to use in current conmmit, because subnet is not contained in Resources/ToolShelfs/SopTools.shelf. If you want to check this feature, add 
a FPMSEdGraphSchemaAction_ShelfTool(UPMSSubGraphNode::StaticClass(),.....) to root menu in the line 317 of PMSEdGraphSchema.cpp.（目前不可用，因为Resources/ToolShelfs/SopTools.shelf中没有subnet，如果你想尝试这一功能，需要在PMSEdGraphSchema.cpp中的317行手动添加一个FPMSEdGraphSchemaAction_ShelfTool(UPMSSubGraphNode::StaticClass(),.....) 到菜单根节点）
