target = NewTarget("test")

function target:configure(settings)
  local tests = CollectDirs(PathJoin(test_dir, "*"))

  for k, v in pairs(tests) do
    local settings = TableDeepCopy(settings)
    local target_path = PathJoin(v, PathFilename(v)..".lua")

    Import(target_path)
    local target_name = PathJoin("test", target.name)
    PseudoTarget(target_name)

    target:configure(TableDeepCopy(settings))
    AddDependency(self.name, target_name)
  end
end
