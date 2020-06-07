#pragma once
#include "common/d3d11/stream_buffer.h"
#include "common/d3d11/staging_texture.h"
#include "common/d3d11/texture.h"
#include "common/windows_headers.h"
#include "core/host_display.h"
#include <SDL.h>
#include <d3d11.h>
#include <memory>
#include <wrl/client.h>

class D3D11HostDisplay final : public HostDisplay
{
public:
  template<typename T>
  using ComPtr = Microsoft::WRL::ComPtr<T>;

  D3D11HostDisplay(SDL_Window* window);
  ~D3D11HostDisplay();

  static std::unique_ptr<HostDisplay> Create(SDL_Window* window, bool debug_device);

  RenderAPI GetRenderAPI() const override;
  void* GetRenderDevice() const override;
  void* GetRenderContext() const override;
  void WindowResized(s32 new_window_width, s32 new_window_height) override;

  std::unique_ptr<HostDisplayTexture> CreateTexture(u32 width, u32 height, const void* data, u32 data_stride,
                                                    bool dynamic) override;
  void UpdateTexture(HostDisplayTexture* texture, u32 x, u32 y, u32 width, u32 height, const void* data,
                     u32 data_stride) override;
  bool DownloadTexture(const void* texture_handle, u32 x, u32 y, u32 width, u32 height, void* out_data,
                       u32 out_data_stride) override;

  void SetVSync(bool enabled) override;

private:
  static constexpr u32 DISPLAY_UNIFORM_BUFFER_SIZE = 16;

  bool CreateD3DDevice(bool debug_device);
  bool CreateD3DResources();
  bool CreateSwapChainRTV();
  bool CreateImGuiContext();

  void Render() override;
  void RenderDisplay();
  void RenderSoftwareCursor();

  SDL_Window* m_window = nullptr;

  ComPtr<IDXGIFactory> m_dxgi_factory;

  ComPtr<ID3D11Device> m_device;
  ComPtr<ID3D11DeviceContext> m_context;
  ComPtr<IDXGISwapChain> m_swap_chain;
  ComPtr<ID3D11RenderTargetView> m_swap_chain_rtv;

  ComPtr<ID3D11RasterizerState> m_display_rasterizer_state;
  ComPtr<ID3D11DepthStencilState> m_display_depth_stencil_state;
  ComPtr<ID3D11BlendState> m_display_blend_state;
  ComPtr<ID3D11BlendState> m_software_cursor_blend_state;
  ComPtr<ID3D11VertexShader> m_display_vertex_shader;
  ComPtr<ID3D11PixelShader> m_display_pixel_shader;
  ComPtr<ID3D11SamplerState> m_point_sampler;
  ComPtr<ID3D11SamplerState> m_linear_sampler;

  D3D11::Texture m_display_pixels_texture;
  D3D11::StreamBuffer m_display_uniform_buffer;
  D3D11::AutoStagingTexture m_readback_staging_texture;

  bool m_allow_tearing_supported = false;
  bool m_vsync = true;
};
