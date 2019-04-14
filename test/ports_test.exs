defmodule PortsTest do
  use ExUnit.Case
  doctest Ports

  @tag :wip
  test "echoes commands" do
    command = 'Testing testing'
    port = Port.open({:spawn, :code.priv_dir(:ports) ++ '/test/echo'}, [{:packet, 2}])
    Port.command(port, command)

    assert_receive {^port, {:data, echoed}}
    assert echoed == command
  end
end
