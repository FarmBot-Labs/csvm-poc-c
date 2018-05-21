defmodule Csvm.ResponsePacket do
  alias Csvm.ResponsePacket
  defstruct [:request_id, :return_status_code, :return_value]

  @typedoc "Request id is a 16 bit integer."
  @type request_id :: 0..65535
  @type return_status_code :: 0..65535
  @type return_value :: 0..65535

  @typedoc "Request Packet type."
  @type t :: %ResponsePacket{
          request_id: request_id,
          return_status_code: return_status_code,
          return_value: return_value
        }

  @doc "Decode a packet."
  @spec decode(binary) :: t
  def decode(binary_data)

  def decode(
        <<request_id::integer-big-size(16), return_status_code::integer-big-size(16),
          return_value::integer-big-size(16)>>
      ) do
    struct(
      ResponsePacket,
      request_id: request_id,
      return_status_code: return_status_code,
      return_value: return_value
    )
  end

  @doc "Encode a packet."
  @spec encode(request_id, return_status_code, return_value) :: binary
  def encode(request_id, return_status_code, return_value) do
    <<request_id::size(16), return_status_code::size(16), return_value::size(16), "\r\n">>
  end
end
