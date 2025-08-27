interface FormItemProps {
  dish_id?: number;
  /** 用于判断是`新增`还是`修改` */
  title: string;
  name: string;
  description: string;
  date: string;
}
interface FormProps {
  formInline: FormItemProps;
}

interface RoleFormItemProps {
  dishname: string;
  description: string;
  date: string;
}
interface RoleFormProps {
  formInline: RoleFormItemProps;
}

export type { FormItemProps, FormProps, RoleFormItemProps, RoleFormProps };
