target = NewTarget("prime")

function target:configure(settings)
  settings.target = self

  local self_dir = PathJoin(test_dir, self.name)
  local self_src_dir = PathJoin(self_dir, "src")

	settings.cc.includes:Add(self_src_dir)
  settings.link.libpath:Add(build_dir)
  settings.link.libs:Add("log")

  local src = CollectRecursive(PathJoin(self_src_dir, "*.cpp"))
  local obj = Compile(settings, src)
  local bin = Link(settings, "test_"..self.name, { obj })

  AddDependency(PathJoin("test", self.name), bin)
end
