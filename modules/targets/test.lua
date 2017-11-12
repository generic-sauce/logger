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

		if target.bin ~= nil then
			local run_target = "run_"..target.name
			PseudoTarget(run_target)
			AddDependency(run_target, target_name)
			AddJob(run_target, "Running test: "..target.name, PathJoin(build_dir, "test_"..target.name))
		end
  end
end
