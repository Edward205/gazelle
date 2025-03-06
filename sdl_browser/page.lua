canvasWidth = getCanvasWidth();
print(canvasWidth)
ui = {}

table.insert(ui, {element = "text", text = "Bine ati venit la portal!\nAceasta pagina contine informatii si demonstratii.", grid = {0, 0}, pos = {0, canvasWidth // 2}})

table.insert(ui, {element = "button", text = "Un buton", grid = {1, 0}, pos = {0, 0}})

table.insert(ui, {element = "button", text = "Alt buton", grid = {2, 0}, pos = {0, 0}})
table.insert(ui, {element = "text", text = "Aliquam rutrum accumsan vulputate.", grid = {2, 2}, pos = {0, 0}})
table.insert(ui, {element = "button", text = "Alt buton", grid = {2, 1}, pos = {0, 0}})


render_page(ui);