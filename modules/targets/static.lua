target = NewTarget("static")

function target:configure(settings)
  settings.target = self

  local src = CollectRecursive(PathJoin(src_dir, "*.cpp"))
  local obj = Compile(settings, src)
  local bin = StaticLibrary(settings, "log", obj)

  AddDependency(self.name, bin)
end
