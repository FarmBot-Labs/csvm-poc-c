defmodule Csvm do
  use GenServer
  require Logger

  def start_link(_) do
    GenServer.start_link(__MODULE__, [], name: __MODULE__)
  end

  def command(packet) do
    GenServer.call(__MODULE__, {:command, packet})
  end

  def init(_) do
    port = open_port()
    {:ok, %{port: port, context: :idle}}
  end

  def terminate(reason, _) do
    Logger.warn("Crash: #{inspect reason}")
  end

  def handle_call({:command, packet}, _, state) do
    Port.command(state.port, packet)
    {:reply, :ok, %{state | context: :sent}}
  end

  def handle_info({_port, {:data, data}}, %{context: :sent} = state) do
    require IEx; IEx.pry
    Csvm.ResponsePacket.decode(data)
    |> IO.inspect(label: "RESPONSE")
    {:noreply, %{state | context: :idle}}
  end

  def handle_info({_port, {:data, data}}, state) do
    Logger.warn "unhandled data: #{inspect data}"
    # Csvm.RequestPacket.decode(data)
    # |> IO.inspect(label: "DATA")

    {:noreply, state}
  end

  def handle_info({_, {:exit_status, status}}, _state) do
    Logger.warn("port exit: #{status}")
    {:noreply, %{port: open_port()}}
  end

  def open_port do
    executable = :code.priv_dir(:csvm) ++ '/csvm'

    port =
      Port.open({:spawn_executable, executable}, [
        {:args, []},
        # {:packet, 2},
        :binary,
        :exit_status
      ])

    port
  end
end
