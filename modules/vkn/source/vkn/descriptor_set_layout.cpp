#include <utility>
#include <vkn/descriptor_set_layout.hpp>

namespace vkn
{
   struct descriptor_set_layout_error_category : std::error_category
   {
      [[nodiscard]] auto name() const noexcept -> const char* override
      {
         return "vkn_descriptor_set_layout";
      }

      [[nodiscard]] auto message(int err) const -> std::string override
      {
         return to_string(static_cast<descriptor_set_layout_error>(err));
      }
   };

   inline static const descriptor_set_layout_error_category descriptor_set_layout_category{};

   auto to_string(descriptor_set_layout_error err) -> std::string
   {
      switch (err)
      {
         case descriptor_set_layout_error::failed_to_create_descriptor_set_layout:
            return "failed_to_create_descriptor_set_layout";
         default:
            return "UNKNOWN";
      }
   }
   auto make_error(descriptor_set_layout_error err, std::error_code ec) -> vkn::error
   {
      return {{static_cast<int>(err), descriptor_set_layout_category},
              static_cast<vk::Result>(ec.value())};
   }

   auto descriptor_set_layout::device() const -> vk::Device { return m_value.getOwner(); }
   auto descriptor_set_layout::bindings() const
      -> const util::dynamic_array<vk::DescriptorSetLayoutBinding>&
   {
      return m_bindings;
   }

   using builder = descriptor_set_layout::builder;

   builder::builder(vk::Device device, std::shared_ptr<util::logger> p_logger) noexcept :
      m_device{device}, mp_logger{std::move(p_logger)}
   {}
   builder::builder(const vkn::device& device, std::shared_ptr<util::logger> p_logger) noexcept :
      m_device{device.value()}, mp_logger{std::move(p_logger)}
   {}

   auto builder::build() const noexcept -> vkn::result<descriptor_set_layout>
   {
      return try_wrap([&] {
                return m_device.createDescriptorSetLayoutUnique(
                   {.bindingCount = static_cast<uint32_t>(std::size(m_info.bindings)),
                    .pBindings = m_info.bindings.data()});
             })
         .map_error([](const vk::SystemError& err) {
            return make_error(descriptor_set_layout_error::failed_to_create_descriptor_set_layout,
                              err.code());
         })
         .map([&](auto handle) {
            util::log_info(mp_logger, "[vkn] descriptor set layout created");

            descriptor_set_layout layout;
            layout.m_value = std::move(handle);
            layout.m_bindings = m_info.bindings;

            return layout;
         });
   }

   auto builder::add_binding(const vk::DescriptorSetLayoutBinding& binding) noexcept -> builder&
   {
      m_info.bindings.emplace_back(binding);
      return *this;
   }

   auto builder::set_bindings(
      const util::dynamic_array<vk::DescriptorSetLayoutBinding>& bindings) noexcept -> builder&
   {
      m_info.bindings = bindings;
      return *this;
   }
} // namespace vkn
