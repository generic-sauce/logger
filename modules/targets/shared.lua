target = NewTarget("shared")

function target:configure(settings)
  settings.target = self

  local settings_obj = TableDeepCopy(settings)
  settings_obj.cc.flags:Add("-fPIC")

  local src = CollectRecursive(PathJoin(src_dir, "*.cpp"))
  local obj = Compile(settings_obj, src)
  local bin = SharedLibrary(settings, "log", obj)

  AddDependency(self.name, bin)
end
